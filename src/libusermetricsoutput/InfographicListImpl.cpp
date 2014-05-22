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

InfographicListImpl::InfographicListImpl(const QString &path, QObject *parent) :
		InfographicList(parent), m_path(path), m_uid(0), m_iterator(
				m_files.cbegin()) {

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(&m_filesystemWatcher, SIGNAL(directoryChanged(const QString &)),
			this, SLOT(internalDirectoryChanged()));

	m_timer.setSingleShot(true);
	m_filesystemWatcher.addPath(path);
	m_workerThread.start();

	updateCurrentPosition();
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
	connect(m_directoryWatcher.data(),
			SIGNAL(directoryChanged(const QString &, const QStringList &)),
			this,
			SLOT(
					directoryContentsChanged(const QString &, const QStringList &)));
	connect(m_directoryWatcher.data(),
			SIGNAL(directoryRemoved(const QString &)), this,
			SLOT(directoryRemoved(const QString &)));
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

void InfographicListImpl::next() {
	if (m_iterator == m_files.cend()) {
		m_iterator = m_files.cbegin();
	} else {
		++m_iterator;
		if (m_iterator == m_files.cend()) {
			m_iterator = m_files.cbegin();
		}
	}

	updateCurrentPosition();
	pathChanged(m_currentFile);
}

static QString foo(const QString &file) {
	QFileInfo info(file);
	QString basename(info.baseName());
	return basename.left(basename.indexOf("-"));
}

void InfographicListImpl::findPreviousPosition() {
	m_iterator = m_files.cbegin();

	if (m_currentInfographic.isEmpty() || m_currentFileIdentity.isEmpty()) {
		return;
	}

	for (auto it = m_files.find(m_currentInfographic);
			it != m_files.cend() && it->first == m_currentInfographic; ++it) {
		QString fileIdentity(foo(it->second));
		if (fileIdentity == m_currentFileIdentity) {
			m_iterator = it;
			break;
		}
	}
}

void InfographicListImpl::updateCurrentPosition() {
	QString infographic;
	QString file;
	if (m_iterator != m_files.cend()) {
		infographic = m_iterator->first;
		file = m_iterator->second;
	}

	if (m_currentInfographic == infographic && m_currentFile == file) {
		return;
	}
	m_currentInfographic = infographic;
	m_currentFile = file;
	m_currentFileIdentity = foo(m_currentFile);
}

QString InfographicListImpl::path() const {
	return m_currentFile;
}

void InfographicListImpl::internalDirectoryChanged() {
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

void InfographicListImpl::directoryContentsChanged(const QString &directory,
		const QStringList &files) {
	m_files.erase(directory);
	for (const QString &file : files) {
		m_files.insert(std::make_pair(directory, file));
	}
	findPreviousPosition();
	updateCurrentPosition();

	m_timer.start(100);
}

void InfographicListImpl::directoryRemoved(const QString &name) {
	m_files.erase(name);
	findPreviousPosition();
	updateCurrentPosition();

	m_timer.start(100);
}

void InfographicListImpl::timeout() {
	if (!m_directoryWatcher) {
		return;
	}

	pathChanged(m_currentFile);
}

