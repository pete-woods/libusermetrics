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

#ifndef USERMETRICSSERVICE_SOURCEDIRECTORYIMPL_H_
#define USERMETRICSSERVICE_SOURCEDIRECTORYIMPL_H_

#include <usermetricsservice/FileUtils.h>
#include <usermetricsservice/SourceDirectory.h>

#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>

namespace UserMetricsService {

class SourceDirectoryImpl: public SourceDirectory {
Q_OBJECT

public:
	SourceDirectoryImpl(const QDir &path, FileUtils::Ptr fileUtils);

	virtual ~SourceDirectoryImpl();

	QStringList files();

protected Q_SLOTS:
	void fileChanged(const QString &path);

	void updateFileList();

protected:
	QDir m_path;

	FileUtils::Ptr m_fileUtils;

	QFileSystemWatcher m_watcher;
};

}

#endif /* USERMETRICSSERVICE_SOURCEDIRECTORYIMPL_H_ */
