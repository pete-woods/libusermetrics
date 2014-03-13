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

#include <usermetricsservice/SourceDirectoryImpl.h>

#include <QDebug>

using namespace UserMetricsCommon;
using namespace UserMetricsService;

SourceDirectoryImpl::SourceDirectoryImpl(const QDir &path,
		FileUtils::Ptr fileUtils) :
		m_path(path), m_fileUtils(fileUtils) {

	m_watcher.addPath(path.path());

	connect(&m_watcher, &QFileSystemWatcher::fileChanged, this,
			&SourceDirectoryImpl::fileChanged);
	connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this,
			&SourceDirectoryImpl::updateFileList);

	updateFileList();
}

SourceDirectoryImpl::~SourceDirectoryImpl() {
}

void SourceDirectoryImpl::fileChanged(const QString &path) {
	// When a file is removed, even though we remove it from the watcher
	// it still triggers a single fileChanged event.
	if (!m_watcher.files().contains(path)) {
		return;
	}

	sourceChanged(m_path, path);
}

void SourceDirectoryImpl::updateFileList() {
	QSet<QString> names(
			m_fileUtils->listDirectory(m_path, QDir::Files).toSet());

	// Remove deleted files
	QSet<QString> toRemove(m_watcher.files().toSet().subtract(names));
	if (!toRemove.isEmpty()) {
		m_watcher.removePaths(toRemove.toList());
	}

	// Work out the names we need to add
	names.subtract(m_watcher.files().toSet());
	for (const QString &name : names) {
		m_watcher.addPath(name);
		sourceChanged(m_path, name);
	}
}

QStringList SourceDirectoryImpl::files() {
	return m_watcher.files();
}
