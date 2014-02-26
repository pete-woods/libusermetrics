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
 */

#include <DBusQuery.h>
#include <MetricInfo.h>

#include <cmath>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QVariantList>

DBusQuery::DBusQuery(QObject *parent) :
		QObject(parent) {
}

static QVariantMap fileContents(int index) {
	QDir cacheDir(TEST_CACHE_DIR);
	QDir usermetricsDir(cacheDir.filePath("usermetrics"));
	QDir sourcesDir(usermetricsDir.filePath("sources"));
	QDir applicationDir(sourcesDir.filePath("test-app"));

	QFileInfoList list(
			applicationDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot,
					QDir::Reversed));
	if (index - 1 >= list.length()) {
		return QVariantMap();
	}
	QFile jsonFile(list.at(index - 1).filePath());

	jsonFile.open(QIODevice::ReadOnly);
	QJsonDocument document(QJsonDocument::fromJson(jsonFile.readAll()));
	jsonFile.close();
	return document.toVariant().toMap();
}

double DBusQuery::queryCurrentValue(int index) {
	return fileContents(index)["data"].toList().at(0).toDouble();
}

MetricInfo* DBusQuery::queryMetricInfo(int index) {
	QVariantMap contents(fileContents(index));
	if (contents.isEmpty()) {
		return nullptr;
	}
	QVariantMap options(contents["options"].toMap());
	double min(nan(""));
	double max(nan(""));
	if (options.contains("minimum")) {
		min = options["minimum"].toDouble();
	}
	if (options.contains("maximum")) {
		max = options["maximum"].toDouble();
	}
	return new MetricInfo(contents["id"].toString(),
			contents["formatString"].toString(),
			contents["emptyDataString"].toString(),
			contents["textDomain"].toString(), min, max, this);
}

