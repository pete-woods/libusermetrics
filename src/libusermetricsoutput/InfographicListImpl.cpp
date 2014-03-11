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

#include <libusermetricscommon/FileUtils.h>
#include <libusermetricsoutput/InfographicListImpl.h>

#include <QtCore/QDebug>
#include <algorithm>

using namespace UserMetricsCommon;
using namespace UserMetricsOutput;

namespace {

static void trim(QSet<QString> &files, const QSet<QString> &directories) {
	for (const QString &directory : directories) {
		auto it(files.begin());
		while (it != files.end()) {
			if (it->startsWith(directory)) {
				it = files.erase(it);
			} else {
				++it;
			}
		}
	}
}

}

InfographicListImpl::InfographicListImpl(const QString &path, QObject *parent) :
		InfographicList(parent), m_path(path), m_uid(0), m_infographics(
				new QStringListModel()) {

	connect(&m_timer, &QTimer::timeout, this, &InfographicListImpl::timeout);
	connect(&m_filesystemWatcher, &QFileSystemWatcher::directoryChanged, this,
			&InfographicListImpl::directoryChanged);

	m_timer.setSingleShot(true);
	m_filesystemWatcher.addPath(path);
	m_workerThread.start();
}

InfographicListImpl::~InfographicListImpl() {
	m_workerThread.quit();
	m_workerThread.wait();
}

unsigned int InfographicListImpl::uid() const {
	return m_uid;
}

void InfographicListImpl::newDirectoryWatcher() {
	m_directoryWatcher.reset(
			new DirectoryWatcher(m_userDir, 1, FileUtils::Ptr(new FileUtils())),
			&QObject::deleteLater);
	m_directoryWatcher->moveToThread(&m_workerThread);
	connect(m_directoryWatcher.data(), &DirectoryWatcher::fileChanged, this,
			&InfographicListImpl::fileChanged);
	connect(m_directoryWatcher.data(), &DirectoryWatcher::directoryRemoved,
			this, &InfographicListImpl::directoryRemoved);
	QTimer::singleShot(0, m_directoryWatcher.data(), SLOT(start()));
}

void InfographicListImpl::setUid(unsigned int uid) {
	if (uid == m_uid) {
		return;
	}

	m_uid = uid;
	m_userDir = m_path.filePath(QString::number(m_uid));

	if (m_userDir.exists()) {
		newDirectoryWatcher();
	}
}

void InfographicListImpl::directoryChanged() {
	if (m_uid == 0) {
		return;
	}

	bool exists(m_userDir.exists());

	if (m_directoryWatcher) {
		if (!exists) {
			m_directoryWatcher.reset();
		}
	} else {
		newDirectoryWatcher();
	}
}

void InfographicListImpl::fileChanged(const QString &name) {
	m_changedFiles.insert(name);
	m_timer.start(100);
}

void InfographicListImpl::directoryRemoved(const QString &name) {
	m_removedDirectories.insert(name);
	trim(m_changedFiles, m_removedDirectories);
	m_timer.start(100);
}

void InfographicListImpl::timeout() {
	if(!m_directoryWatcher) {
		return;
	}

	trim(m_files, m_removedDirectories);

	// There's no point giving updates for new SVGs, so
	// this will be the set of files that we already know
	// about, but has changed on disk.
	QSet<QString> changed(m_changedFiles);
	changed.intersect(m_files);

	// Add changed files to known files
	m_files.unite(m_changedFiles);

	// Don't need these any more
	m_removedDirectories.clear();
	m_changedFiles.clear();

	QSet<QString> currentFiles(m_infographics->stringList().toSet());
	if (currentFiles != m_files) {
		m_infographics->setStringList(m_files.toList());
	}

	if (!changed.isEmpty()) {
		filesUpdated(changed.toList());
	}
}

QAbstractItemModel * InfographicListImpl::infographics() const {
	return m_infographics.data();
}

