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

#ifndef USERMETRICSSERVICE_SOURCEDIRECTORY_H_
#define USERMETRICSSERVICE_SOURCEDIRECTORY_H_

#include <QDir>
#include <QObject>
#include <QSharedPointer>

namespace UserMetricsService {

class SourceDirectory: public QObject {
Q_OBJECT
public:
	typedef QSharedPointer<SourceDirectory> Ptr;

	SourceDirectory();

	virtual ~SourceDirectory();

	virtual QStringList files() = 0;

Q_SIGNALS:
	void sourceChanged(const QDir &directory, const QString &file);
};

}

#endif /* USERMETRICSSERVICE_SOURCEDIRECTORY_H_ */
