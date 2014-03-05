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

#include <libusermetricscommon/FileUtils.h>

#include <QRegularExpression>

using namespace UserMetricsCommon;

static QRegularExpression CLICK_REGEX(
		"^[a-z0-9][a-z0-9+.-]+_[a-zA-Z0-9+.-]+_[0-9][a-zA-Z0-9.+:~-]*$");

FileUtils::FileUtils() {
}

FileUtils::~FileUtils() {
}

QSet<QString> FileUtils::listDirectory(const QDir &directory,
		QDir::Filters filters) {
	QSet<QString> names;
	for (const QFileInfo& fileInfo : directory.entryInfoList(
			filters | QDir::NoDotAndDotDot)) {
		names << fileInfo.filePath();
	}
	return names;
}

void FileUtils::shortApplicationId(QString &applicationId) {
	QRegularExpressionMatch match = CLICK_REGEX.match(applicationId);
	if (match.hasMatch()) {
		int index = applicationId.indexOf('_');
		if (index != -1) {
			applicationId.remove(index, applicationId.size());
		}
	}
}
