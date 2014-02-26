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

#include <usermetricsservice/InfographicFileImpl.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>

using namespace UserMetricsService;

InfographicFileImpl::InfographicFileImpl(const QFile &path,
		const Service &service, Factory &factory) :
		m_path(path.fileName()), m_factory(factory) {

	if (!m_path.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open path:" << m_path.fileName();
		return;
	}

	QByteArray ba(m_path.readAll());
	m_path.close();

	QJsonParseError error;
	QJsonDocument document(QJsonDocument::fromJson(ba, &error));
	if (error.error != QJsonParseError::NoError) {
		qWarning() << "Failed to parse infographic JSON document:"
				<< m_path.fileName() << error.errorString();
		return;
	}

	if (document.isNull() || document.isEmpty()) {
		return;
	}

	for (const QVariant &config : document.toVariant().toList()) {
		Infographic::Ptr infographic(m_factory.newInfographic(config));
		if (infographic->isValid()) {
			QObject::connect(&service, &Service::sourcesChanged,
					infographic.data(), &Infographic::sourcesChanged);
			m_infographics << infographic;
		}
	}
}

InfographicFileImpl::~InfographicFileImpl() {
}
