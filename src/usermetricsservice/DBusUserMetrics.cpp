/*
 * Copyright (C) 2013 Canonical, Ltd.
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

#include <usermetricsservice/DBusDataSource.h>
#include <usermetricsservice/DBusUserMetrics.h>
#include <usermetricsservice/DBusUserData.h>
#include <usermetricsservice/UserMetricsAdaptor.h>
#include <usermetricsservice/database/DataSource.h>
#include <usermetricsservice/database/UserData.h>
#include <usermetricsservice/database/DataSet.h>
#include <libusermetricscommon/DateFactory.h>
#include <libusermetricscommon/DBusPaths.h>

#include <QDjango.h>
#include <QDjangoQuerySet.h>

using namespace UserMetricsCommon;
using namespace UserMetricsService;

DBusUserMetrics::DBusUserMetrics(QDBusConnection &dbusConnection,
		QSharedPointer<DateFactory> dateFactory, QObject *parent) :
		QObject(parent), m_dbusConnection(dbusConnection), m_adaptor(
				new UserMetricsAdaptor(this)), m_dateFactory(dateFactory) {

	// Database setup
	QDjango::registerModel<DataSource>();
	QDjango::registerModel<UserData>();
	QDjango::registerModel<DataSet>();

	QDjango::createTables();

	// DBus setup
	m_dbusConnection.registerService(DBusPaths::serviceName());
	m_dbusConnection.registerObject(DBusPaths::userMetrics(), this);

	syncDatabase();
}

DBusUserMetrics::~DBusUserMetrics() {
	m_dbusConnection.unregisterObject("/com/canonical/UserMetrics");
	m_dbusConnection.unregisterService("com.canonical.UserMetrics");
}

QList<QDBusObjectPath> DBusUserMetrics::dataSources() const {
	QList<QDBusObjectPath> dataSources;
	for (DBusDataSourcePtr dataSource : m_dataSources.values()) {
		dataSources << QDBusObjectPath(dataSource->path());
	}
	return dataSources;
}

void DBusUserMetrics::syncDatabase() {
	{
		QSet<int> dataSourceNames;
		QDjangoQuerySet<DataSource> query;
		for (const DataSource &dataSource : query) {
			const int id(dataSource.id());
			dataSourceNames << id;
			// if we don't have a local cache
			if (!m_dataSources.contains(id)) {
				qDebug() << "DBusUserMetrics::syncDatabase create DataSource("
						<< id << ")";
				DBusDataSourcePtr dbusDataSource(
						new DBusDataSource(id, m_dbusConnection));
				m_dataSources.insert(id, dbusDataSource);
			}
		}
		// remove any cached references to deleted sources
		QSet<int> cachedDataSourceNames(
				QSet<int>::fromList(m_dataSources.keys()));
		QSet<int> &toRemove(cachedDataSourceNames.subtract(dataSourceNames));
		for (int id : toRemove) {
			m_dataSources.remove(id);
		}
	}

	{
		QSet<int> usernames;
		QDjangoQuerySet<UserData> query;
		for (const UserData &userData : query) {
			const int id(userData.id());
			usernames << id;
			// if we don't have a local cache
			if (!m_userData.contains(id)) {
				qDebug() << "DBusUserMetrics::syncDatabase create UserData("
						<< id << ")";
				DBusUserDataPtr dbusUserData(
						new DBusUserData(id, *this, m_dbusConnection,
								m_dateFactory));
				m_userData.insert(id, dbusUserData);
			}
		}
		// remove any cached references to deleted sources
		QSet<int> cachedUsernames(QSet<int>::fromList(m_userData.keys()));
		QSet<int> &toRemove(cachedUsernames.subtract(usernames));
		for (int id : toRemove) {
			m_userData.remove(id);
		}
	}
}

QDBusObjectPath DBusUserMetrics::createDataSource(const QString &name,
		const QString &formatString) {
	qDebug() << "createDataSource(" << name << formatString << ")";
	QDjangoQuerySet<DataSource> dataSourcesQuery;
	QDjangoQuerySet<DataSource> query(
			dataSourcesQuery.filter(
					QDjangoWhere("name", QDjangoWhere::Equals, name)));

	Q_ASSERT(query.size() != -1);

	int id;

	if (query.size() == 0) {
		DataSource dataSource;
		dataSource.setName(name);
		dataSource.setFormatString(formatString);
		Q_ASSERT(dataSource.save());

		id = dataSource.id();

		syncDatabase();
	} else {
		DataSource dataSource;
		query.at(0, &dataSource);

		id = dataSource.id();
	}

	return QDBusObjectPath((*m_dataSources.constFind(id))->path());
}

QList<QDBusObjectPath> DBusUserMetrics::userDatas() const {
	QList<QDBusObjectPath> userDatas;
	for (DBusUserDataPtr userData : m_userData.values()) {
		userDatas << QDBusObjectPath(userData->path());
	}
	return userDatas;
}

QDBusObjectPath DBusUserMetrics::createUserData(const QString &username) {
	qDebug() << "createUserData(" << username << ")";

	QDjangoQuerySet<UserData> query(
			QDjangoQuerySet<UserData>().filter(
					QDjangoWhere("username", QDjangoWhere::Equals, username)));

	Q_ASSERT(query.size() != -1);

	int id;

	if (query.size() == 0) {
		UserData userData;
		userData.setUsername(username);
		Q_ASSERT(userData.save());

		id = userData.id();

		qDebug() << "-> new data with id(" << id << ")";

		syncDatabase();
	} else {
		UserData userData;
		query.at(0, &userData);

		qDebug() << "-> existing data with id(" << id << ")";

		id = userData.id();
	}

	return QDBusObjectPath((*m_userData.constFind(id))->path());
}

DBusDataSourcePtr DBusUserMetrics::dataSource(const QString &name) const {
	DataSource dataSource;
	DataSource::findByName(name, &dataSource);

	return m_dataSources.value(dataSource.id());
}

DBusUserDataPtr DBusUserMetrics::userData(const QString &username) const {
	UserData userData;
	UserData::findByName(username, &userData);

	return m_userData.value(userData.id());
}
