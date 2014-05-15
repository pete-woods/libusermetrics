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

#include <usermetricsservice/QProcessExecutor.h>

#include <QDebug>
#include <QProcess>
#include <QThread>

using namespace UserMetricsService;

QProcessExecutor::QProcessExecutor(const QDir &cacheDirectory,
		const QString &aaExec) :
		m_aaExec(aaExec) {

	m_usermetricsDirectory = cacheDirectory.filePath("usermetrics");
	m_tempDirectory = m_usermetricsDirectory.filePath("tmp");
}

QProcessExecutor::~QProcessExecutor() {
}

QByteArray QProcessExecutor::execute(const QString &program,
		const QString &profile, const QStringList &arguments) {

	m_tempDirectory.removeRecursively();
	m_usermetricsDirectory.mkpath("tmp");

	QByteArray output;

	QStringList finalArguments;
	finalArguments << profile;
	finalArguments << program;
	for (int i(0); i < arguments.size(); ++i) {
		const QString &argument(arguments[i]);
		QString destination(
				m_tempDirectory.filePath(QString::number(i) + ".svg"));
		if (!QFile::copy(argument, destination)) {
			qWarning() << "Failed to copy file" << argument << destination;
			return output;
		}
		finalArguments << destination;
	}

	QProcess process;

	qDebug() << "Starting process:" << m_aaExec << finalArguments;
	process.start(m_aaExec, finalArguments);
	qDebug () << process.waitForStarted();

	if (process.waitForFinished(5000)) {
		output = process.readAllStandardOutput();
		QByteArray error(process.readAllStandardError());
		if (!error.isEmpty()) {
			qWarning() << error;
		}
	} else {
		process.kill();
	}

	return output;
}

