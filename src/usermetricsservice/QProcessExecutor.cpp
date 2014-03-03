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

#include <usermetricsservice/QProcessExecutor.h>

#include <QDebug>
#include <QProcess>
#include <QThread>

using namespace UserMetricsService;

QProcessExecutor::QProcessExecutor() {
}

QProcessExecutor::~QProcessExecutor() {
}

QByteArray QProcessExecutor::execute(const QString &program,
		const QStringList &arguments) {

	QByteArray output;
	QProcess process;

	process.start(program, arguments);

	if (process.waitForFinished(5000)) {
		output = process.readAllStandardOutput();
	} else {
		process.kill();
	}

	return output;
}

