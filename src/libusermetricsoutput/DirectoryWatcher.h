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

#ifndef USERMETRICSOUTPUT_DIRECTORYWATCHER_H_
#define USERMETRICSOUTPUT_DIRECTORYWATCHER_H_

#include <libusermetricscommon/FileUtils.h>

#include <QDir>
#include <QMap>
#include <QFileSystemWatcher>
#include <QSharedPointer>

namespace UserMetricsOutput {

class DirectoryWatcher: public QObject {
Q_OBJECT

public:
	typedef QSharedPointer<DirectoryWatcher> Ptr;

	DirectoryWatcher(const QDir &path, unsigned int maxDepth,
			UserMetricsCommon::FileUtils::Ptr fileUtils);

	virtual ~DirectoryWatcher();

	QStringList files();

public Q_SLOTS:
	void start();

Q_SIGNALS:
	void fileChanged(const QString &file);

	void directoryRemoved(const QString &directory);

protected Q_SLOTS:
	void internalFileChanged(const QString &path);

	void internalDirectoryChanged();

protected:
	QDir m_path;

	unsigned int m_maxDepth;

	UserMetricsCommon::FileUtils::Ptr m_fileUtils;

	QFileSystemWatcher m_watcher;

	QMap<QString, DirectoryWatcher::Ptr> m_directories;
};

}

#endif /* USERMETRICSOUTPUT_DIRECTORYWATCHER_H_ */
