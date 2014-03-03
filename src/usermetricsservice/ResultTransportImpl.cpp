/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <usermetricsservice/ResultTransportImpl.h>

#include <QDebug>
#include <sys/stat.h>

using namespace UserMetricsService;

ResultTransportImpl::ResultTransportImpl(
		QSharedPointer<ComCanonicalInfographicsInterface> infographicService) :
		m_infographicService(infographicService) {
}

ResultTransportImpl::~ResultTransportImpl() {
}

void ResultTransportImpl::send(const QString &id, const QStringList &arguments,
		const QByteArray &ba) {
	QFile fifo;
	{
		QTemporaryFile tempFile;
		tempFile.open();
		tempFile.close();
		fifo.setFileName(tempFile.fileName());
	}

	int err = mkfifo(fifo.fileName().toUtf8().constData(), 0644);
	if (err != 0) {
		// someone is trying something naughty
		qWarning() << "FIFO path already existed" << fifo.fileName();
		return;
	}

	QDBusPendingReply<> reply(
			m_infographicService->update(id, arguments, fifo.fileName()));

	if (fifo.open(QIODevice::WriteOnly)) {
		fifo.write(ba);
		fifo.close();
	}

	reply.waitForFinished();

	fifo.remove();
}
