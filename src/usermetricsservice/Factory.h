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

#ifndef USERMETRICSSERVICE_FACTORY_H_
#define USERMETRICSSERVICE_FACTORY_H_

#include <libusermetricscommon/FileUtils.h>
#include <libusermetricscommon/InfographicsInterface.h>
#include <usermetricsservice/Executor.h>
#include <usermetricsservice/Infographic.h>
#include <usermetricsservice/ResultTransport.h>
#include <usermetricsservice/Service.h>
#include <usermetricsservice/SourceDirectory.h>

#include <QFile>
#include <QDir>

namespace UserMetricsService {

class Factory {
public:
	Factory();

	virtual ~Factory();

	virtual Service::Ptr singletonService();

	virtual UserMetricsCommon::FileUtils::Ptr singletonFileUtils();

	virtual Executor::Ptr singletonExecutor();

	virtual QSharedPointer<ComCanonicalInfographicsInterface> singletonInfographicService();

	virtual ResultTransport::Ptr singletonResultTransport();

	virtual Infographic::Ptr newInfographic(const QFile &path);

	virtual SourceDirectory::Ptr newSourceDirectory(const QDir &path);

protected:
	Service::Ptr m_service;

	UserMetricsCommon::FileUtils::Ptr m_fileUtils;

	Executor::Ptr m_executor;

	QSharedPointer<ComCanonicalInfographicsInterface> m_infographicService;

	ResultTransport::Ptr m_resultTransport;
};

}

#endif /* USERMETRICSSERVICE_FACTORY_H_ */
