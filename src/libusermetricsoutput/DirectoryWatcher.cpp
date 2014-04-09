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

namespace {

static bool nameCompare(const QString &a, const QString &b) {
	int indexA = a.lastIndexOf('-');
	int indexB = b.lastIndexOf('-');

	if (indexA == -1 || indexB == -1) {
		return false;
	}

	QStringRef startA = a.leftRef(indexA);
	QStringRef startB = b.leftRef(indexB);

	return !!startA.compare(startB);
}

}

DirectoryWatcher::DirectoryWatcher(const QDir &path, unsigned int maxDepth,
		FileUtils::Ptr fileUtils) :
		m_path(path), m_maxDepth(maxDepth), m_fileUtils(fileUtils) {

	m_watcher.addPath(path.path());

	connect(&m_watcher, SLOT(directoryChanged()), this,
			SIGNAL(internalDirectoryChanged()));
}

DirectoryWatcher::~DirectoryWatcher() {
}

void DirectoryWatcher::start() {
	internalDirectoryChanged();
}

void DirectoryWatcher::internalDirectoryChanged() {
	QStringList files(
			m_fileUtils->listDirectory(m_path, QDir::Files, QDir::Name));

	QStringList newestFiles;

	const QString *previous(nullptr);
	for (const QString &file : files) {
		if (!previous) {
			previous = &file;
		} else if (nameCompare(*previous, file)) {
			newestFiles << *previous;
		}
		previous = &file;
	}

	if (previous) {
		newestFiles << *previous;
	}

	directoryChanged(m_path.path(), newestFiles);

	if (m_maxDepth == 0) {
		return;
	}

	QSet<QString> directories(
			m_fileUtils->listDirectory(m_path, QDir::Dirs).toSet());

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
		connect(watcher.data(), SLOT(directoryChanged()), this,
				SIGNAL(directoryChanged()));
		connect(watcher.data(), SLOT(directoryRemoved()), this,
				SIGNAL(directoryRemoved()));
		watcher->start();
	}
}

QStringList DirectoryWatcher::files() {
	return m_watcher.files();
}
