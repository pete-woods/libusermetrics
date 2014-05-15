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

#include <libusermetricscommon/DBusPaths.h>
#include <usermetricsservice/Factory.h>
#include <usermetricsservice/InfographicImpl.h>
#include <usermetricsservice/QProcessExecutor.h>
#include <usermetricsservice/ResultTransportImpl.h>
#include <usermetricsservice/SourceDirectoryImpl.h>
#include <usermetricsservice/ServiceImpl.h>

using namespace UserMetricsCommon;
using namespace UserMetricsService;

Factory::Factory() {
}

Factory::~Factory() {
}

Service::Ptr Factory::singletonService() {
	if (!m_service) {
		m_service.reset(
				new ServiceImpl(QDir::home().filePath(".local/share/libusermetrics"),
						QDir::home().filePath(".cache"),
						QDir("/usr/share/libusermetrics/infographics"),
						singletonFileUtils(), singletonResultTransport(),
						*this));
	}
	return m_service;
}

FileUtils::Ptr Factory::singletonFileUtils() {
	if (!m_fileUtils) {
		m_fileUtils.reset(new FileUtils());
	}
	return m_fileUtils;
}

Executor::Ptr Factory::singletonExecutor() {
	if (!m_executor) {
		m_executor.reset(new QProcessExecutor(QDir::home().filePath(".cache"),
		AA_EXEC_PATH));
	}
	return m_executor;
}

QSharedPointer<ComCanonicalInfographicsInterface> Factory::singletonInfographicService() {
	if (!m_infographicService) {
		m_infographicService.reset(
				new ComCanonicalInfographicsInterface(
						DBusPaths::INFOGRAPHIC_DBUS_NAME,
						DBusPaths::INFOGRAPHIC_DBUS_PATH,
						QDBusConnection::systemBus()));
	}
	return m_infographicService;
}

ResultTransport::Ptr Factory::singletonResultTransport() {
	if (!m_resultTransport) {
		m_resultTransport.reset(
				new ResultTransportImpl(singletonInfographicService()));
	}
	return m_resultTransport;
}

Infographic::Ptr Factory::newInfographic(const QFile &path, bool click) {
	return Infographic::Ptr(
			new InfographicImpl(path, click, singletonExecutor(),
					singletonResultTransport()));
}

SourceDirectory::Ptr Factory::newSourceDirectory(const QDir &path) {
	return SourceDirectory::Ptr(
			new SourceDirectoryImpl(path, singletonFileUtils()));
}
