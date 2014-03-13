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

#ifndef USERMETRICSOUTPUT_INFOGRAPHICLISTIMPL_H_
#define USERMETRICSOUTPUT_INFOGRAPHICLISTIMPL_H_

#include <libusermetricsoutput/DirectoryWatcher.h>
#include <libusermetricsoutput/InfographicList.h>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QMultiMap>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QScopedPointer>

namespace UserMetricsOutput {

class InfographicListImpl: public InfographicList {
Q_OBJECT

public:
	InfographicListImpl(const QString &path, QObject *parent = 0);

	virtual ~InfographicListImpl();

	unsigned int uid() const override;

	void setUid(unsigned int uid) override;

protected Q_SLOTS:
	void internalDirectoryChanged();

	void directoryContentsChanged(const QString &directory,
			const QStringList &files);

	void directoryRemoved(const QString &directory);

	void timeout();

protected:
	void newDirectoryWatcher();

	QThread m_workerThread;

	QTimer m_timer;

	QDir m_path;

	QDir m_userDir;

	unsigned int m_uid;

	QFileSystemWatcher m_filesystemWatcher;

	DirectoryWatcher::Ptr m_directoryWatcher;

	QMultiMap<QString, QString> m_files;
};

}

#endif /* USERMETRICSOUTPUT_INFOGRAPHICLISTIMPL_H_ */
