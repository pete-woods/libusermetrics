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

#ifndef USERMETRICSCOMMON_FILEUTILS_H_
#define USERMETRICSCOMMON_FILEUTILS_H_

#include <QDir>
#include <QSet>
#include <QSharedPointer>
#include <QStringList>

namespace UserMetricsCommon {

class FileUtils {
public:
	typedef QSharedPointer<FileUtils> Ptr;

	FileUtils();

	virtual ~FileUtils();

	virtual QSet<QString> listDirectory(const QDir &directory,
			QDir::Filters filters);

	virtual QSet<QString> listDirectory(const QStringList &nameFilters,
			const QDir &directory, QDir::Filters filters);

	void shortApplicationId(QString &applicationId);
};

}

#endif /* USERMETRICSCOMMON_FILEUTILS_H_ */
