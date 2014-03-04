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

#include <usermetricsservice/ServiceImpl.h>

#include <QDebug>
#include <QStringList>
#include <stdexcept>

using namespace UserMetricsService;
using namespace std;

ServiceImpl::ServiceImpl(const QDir &cacheDirectory,
		const QDir &packageInfographics, FileUtils::Ptr fileUtils,
		ResultTransport::Ptr resultTransport, Factory &factory) :
		m_cacheDirectory(cacheDirectory), m_fileUtils(fileUtils), m_resultTransport(
				resultTransport), m_factory(factory) {
	QDir usermetricsDirectory(cacheDirectory.filePath("usermetrics"));

	if (!usermetricsDirectory.mkpath("infographics")) {
		throw logic_error("Cannot create infographics directory");
	}

	if (!usermetricsDirectory.mkpath("sources")) {
		throw logic_error("Cannot create sources directory");
	}

	m_infographicDirectories << usermetricsDirectory.filePath("infographics");
	m_infographicDirectories << packageInfographics.path();
	m_sourcesDirectory = usermetricsDirectory.filePath("sources");

	m_timer.setSingleShot(true);

	connect(&m_infographicWatcher, &QFileSystemWatcher::directoryChanged, this,
	&ServiceImpl::updateInfographicList);
	connect(&m_sourcesWatcher, &QFileSystemWatcher::directoryChanged, this,
	&ServiceImpl::updateSourcesList);

	connect(&m_timer, &QTimer::timeout, this, &ServiceImpl::flushQueue);

	updateInfographicList();
	updateSourcesList();

	m_infographicWatcher.addPaths(m_infographicDirectories);
	m_sourcesWatcher.addPath(m_sourcesDirectory.path());


	m_resultTransport->clear();

	QMultiMap<QString, QString> sources(allSources());
	sourcesChanged(sources, sources);
}

ServiceImpl::~ServiceImpl() {
}

void ServiceImpl::updateInfographicList() {
	QSet<QString> names;

	for (const QString &directory : m_infographicDirectories) {
		names.unite(m_fileUtils->listDirectory(directory, QDir::Files));
	}

	// Remove deleted infographics
	QSet<QString> toRemove(m_infographics.keys().toSet().subtract(names));
	for (const QString &name : toRemove) {
		m_infographics.take(name);
	}

	// Work out the names we need to add
	names.subtract(m_infographics.keys().toSet());
	for (const QString &name : names) {
		Infographic::Ptr infographic(m_factory.newInfographic(name));
		m_infographics.insert(name, infographic);
		if (infographic->isValid()) {
			connect(this, &Service::sourcesChanged, infographic.data(),
					&Infographic::sourcesChanged);
		}
	}
}

void ServiceImpl::updateSourcesList() {
	QSet<QString> fullNames(
			m_fileUtils->listDirectory(m_sourcesDirectory, QDir::Files));

	QSet<QString> names;
	for (const QString &name : fullNames) {
		QFileInfo fileInfo(name);
		QString shortName(fileInfo.completeBaseName());
		m_fileUtils->shortApplicationId(shortName);
		QDir applicationDirectory(m_cacheDirectory.filePath(shortName));
		applicationDirectory.mkpath("usermetrics");
		names << applicationDirectory.filePath("usermetrics");
	}

	// Remove deleted sources
	QSet<QString> toRemove(m_sources.keys().toSet().subtract(names));
	for (const QString &name : toRemove) {
		m_sources.take(name);
	}

	// Work out the names we need to add
	names.subtract(m_sources.keys().toSet());
	for (const QString &name : names) {
		SourceDirectory::Ptr sourceDirectory(
				m_factory.newSourceDirectory(name));
		connect(sourceDirectory.data(), &SourceDirectory::sourceChanged, this,
				&ServiceImpl::sourceChanged);
		m_sources.insert(name, sourceDirectory);
	}
}

void ServiceImpl::sourceChanged(const QDir &directory,
		const QString &fileName) {
	QFileInfo directoryInfo(directory.path());
	QString applicationId(directoryInfo.dir().dirName());
	m_fileUtils->shortApplicationId(applicationId);

	if (m_changedSources.constFind(applicationId, fileName)
			== m_changedSources.constEnd()) {
		m_changedSources.insert(applicationId, fileName);
	}
	m_timer.start(100);
}

QMultiMap<QString, QString> ServiceImpl::allSources() {
	QMultiMap<QString, QString> sources;

	QMapIterator<QString, SourceDirectory::Ptr> iter(m_sources);
	while (iter.hasNext()) {
		iter.next();

		QFileInfo directoryInfo(iter.key());
		QString applicationId(directoryInfo.dir().dirName());
		m_fileUtils->shortApplicationId(applicationId);

		for (const QString &filePath : iter.value()->files()) {
			sources.insert(applicationId, filePath);
		}
	}

	return sources;
}

void ServiceImpl::flushQueue() {
	sourcesChanged(m_changedSources, allSources());
	m_changedSources.clear();
}
