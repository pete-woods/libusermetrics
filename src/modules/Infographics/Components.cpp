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

#include <QtQuick/QtQuick>
#include <modules/UserMetrics/Components.h>
#include <libusermetricsoutput/InfographicList.h>

static QObject *infographicsProvider(QQmlEngine *engine,
		QJSEngine *scriptEngine) {
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
	return UserMetricsOutput::InfographicList::getInstance();
}

void Components::registerTypes(const char *uri) {
	qmlRegisterSingletonType<UserMetricsOutput::InfographicList>(uri, 0, 1,
			"InfographicList", infographicsProvider);
}

void Components::initializeEngine(QQmlEngine *engine, const char *uri) {
	QQmlExtensionPlugin::initializeEngine(engine, uri);
}
