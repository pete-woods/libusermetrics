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

#include <usermetricsservice/Factory.h>
#include <usermetricsservice/InfographicFileImpl.h>
#include <usermetricsservice/InfographicImpl.h>
#include <usermetricsservice/SourceDirectoryImpl.h>
#include <usermetricsservice/ServiceImpl.h>

using namespace UserMetricsService;

Factory::Factory() {
}

Factory::~Factory() {
}

Service::Ptr Factory::singletonService() {
	if (!m_service) {
		m_service.reset(
				new ServiceImpl(QDir::home().filePath(".cache"),
						singletonFileUtils(), *this));
	}
	return m_service;
}

FileUtils::Ptr Factory::singletonFileUtils() {
	if (!m_fileUtils) {
		m_fileUtils.reset(new FileUtils());
	}
	return m_fileUtils;
}

InfographicFile::Ptr Factory::newInfographicFile(const QFile &path,
		const Service &service) {
	return InfographicFile::Ptr(new InfographicFileImpl(path, service, *this));
}

Infographic::Ptr Factory::newInfographic(const QVariant &config) {
	return Infographic::Ptr(new InfographicImpl(config));
}

SourceDirectory::Ptr Factory::newSourceDirectory(const QDir &path) {
	return SourceDirectory::Ptr(
			new SourceDirectoryImpl(path, singletonFileUtils()));
}
