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
		InfographicList(parent), m_path(path), m_uid(0) {

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(&m_filesystemWatcher, SIGNAL(directoryChanged()), this,
			SLOT(internalDirectoryChanged()));

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
	connect(m_directoryWatcher.data(), SIGNAL(directoryChanged()),
			this, SLOT(directoryContentsChanged()));
	connect(m_directoryWatcher.data(), SIGNAL(directoryRemoved()),
			this, SLOT(directoryRemoved()));
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
	m_files.remove(directory);
	for (const QString &file : files) {
		m_files.insert(directory, file);
	}
	m_timer.start(100);
}

void InfographicListImpl::directoryRemoved(const QString &name) {
	m_files.remove(name);
	m_timer.start(100);
}

void InfographicListImpl::timeout() {
	if (!m_directoryWatcher) {
		return;
	}

	int existingRows(rowCount());
	int newRows(m_files.count());

	// Special case the initial insert
	if (existingRows == 0 && newRows > 0) {
		setStringList(m_files.values());
		return;
	}

	if (newRows > existingRows) {
		// insert new rows
		insertRows(existingRows, newRows - existingRows);
	} else if (newRows < existingRows) {
		// remove extra rows
		removeRows(newRows, existingRows - newRows);
	}

	int row(0);
	QMapIterator<QString, QString> it(m_files);
	while (it.hasNext()) {
		it.next();
		const QString &file(it.value());

		QString existing(data(index(row, 0), Qt::DisplayRole).toString());
		if (existing != file) {
			setData(index(row, 0), file);
		}
		++row;
	}
}

