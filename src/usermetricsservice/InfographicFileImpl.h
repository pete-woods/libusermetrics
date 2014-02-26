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

#ifndef USERMETRICSSERVICE_INFOGRAPHICFILEIMPL_H_
#define USERMETRICSSERVICE_INFOGRAPHICFILEIMPL_H_

#include <usermetricsservice/Factory.h>
#include <usermetricsservice/Infographic.h>
#include <usermetricsservice/InfographicFile.h>

#include <QFile>

namespace UserMetricsService {

class InfographicFileImpl: public InfographicFile {
public:
	InfographicFileImpl(const QFile &path, const Service &service,
			Factory &factory);

	virtual ~InfographicFileImpl();

protected:
	QFile m_path;

	Factory &m_factory;

	QList<Infographic::Ptr> m_infographics;
};

}

#endif /* USERMETRICSSERVICE_INFOGRAPHICFILEIMPL_H_ */
