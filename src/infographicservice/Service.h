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

#ifndef INFOGRAPHICSERVICE_SERVICE_H_
#define INFOGRAPHICSERVICE_SERVICE_H_

#include <QCryptographicHash>
#include <QDir>
#include <QDBusContext>
#include <QDBusConnection>
#include <QObject>
#include <QSharedPointer>

class InfographicsAdaptor;

namespace InfographicService {

class Service: public QObject, protected QDBusContext {
Q_OBJECT

public:
	Service(const QDir &directory, const QDBusConnection &systemConnection);

	virtual ~Service();

public Q_SLOTS:
	void clear();

	void update(const QString &visualizer, const QStringList &sources,
			const QString &file);

protected:
	virtual unsigned int uid();

	QDir userDirectory();

	QDir m_directory;

	QDBusConnection m_connection;

	QSharedPointer<InfographicsAdaptor> m_adaptor;

	QCryptographicHash m_hash;
};

}

#endif /* INFOGRAPHICSERVICE_SERVICE_H_ */
