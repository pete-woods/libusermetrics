/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <libusermetricsoutput/DirectoryWatcher.h>

#include <QDebug>

using namespace UserMetricsCommon;
using namespace UserMetricsOutput;

DirectoryWatcher::DirectoryWatcher(const QDir &path, unsigned int maxDepth,
		FileUtils::Ptr fileUtils) :
		m_path(path), m_maxDepth(maxDepth), m_fileUtils(fileUtils) {

	m_watcher.addPath(path.path());

	connect(&m_watcher, &QFileSystemWatcher::fileChanged, this,
			&DirectoryWatcher::internalFileChanged);
	connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this,
			&DirectoryWatcher::internalDirectoryChanged);
}

DirectoryWatcher::~DirectoryWatcher() {
}

void DirectoryWatcher::start() {
	internalDirectoryChanged();
}

void DirectoryWatcher::internalFileChanged(const QString &path) {
	// When a file is removed, even though we remove it from the watcher
	// it still triggers a single fileChanged event.
	if (!m_watcher.files().contains(path)) {
		return;
	}

	fileChanged(path);
}

void DirectoryWatcher::internalDirectoryChanged() {
	QSet<QString> files(m_fileUtils->listDirectory(m_path, QDir::Files));

	// Remove deleted files
	QSet<QString> filesToRemove(m_watcher.files().toSet().subtract(files));
	if (!filesToRemove.isEmpty()) {
		m_watcher.removePaths(filesToRemove.toList());
	}

	// Work out the names we need to add
	files.subtract(m_watcher.files().toSet());
	for (const QString &name : files) {
		m_watcher.addPath(name);
		fileChanged(name);
	}

	if (m_maxDepth == 0) {
		return;
	}

	QSet<QString> directories(m_fileUtils->listDirectory(m_path, QDir::Dirs));

	// Remove deleted files
	QSet<QString> directoriesToRemove(
			m_directories.keys().toSet().subtract(directories));
	for (const QString &name : directoriesToRemove) {
		m_directories.remove(name);
		directoryRemoved(name);
	}

	// Work out the names we need to add
	directories.subtract(m_directories.keys().toSet());
	for (const QString &name : directories) {
		DirectoryWatcher::Ptr watcher(
				new DirectoryWatcher(name, m_maxDepth - 1, m_fileUtils));
		m_directories.insert(name, watcher);
		connect(watcher.data(), &DirectoryWatcher::fileChanged, this,
				&DirectoryWatcher::fileChanged);
		connect(watcher.data(), &DirectoryWatcher::directoryRemoved, this,
				&DirectoryWatcher::directoryRemoved);
		watcher->start();
	}
}

QStringList DirectoryWatcher::files() {
	return m_watcher.files();
}
