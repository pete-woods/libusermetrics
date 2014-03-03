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

#ifndef USERMETRICSSERVICE_QPROCESSEXECUTOR_H_
#define USERMETRICSSERVICE_QPROCESSEXECUTOR_H_

#include <usermetricsservice/Executor.h>

#include <QDir>

namespace UserMetricsService {

class QProcessExecutor: public Executor {
public:
	QProcessExecutor(const QDir &cacheDirectory, const QString &aaExec);

	virtual ~QProcessExecutor();

	QByteArray execute(const QString &program, const QStringList &arguments);

protected:
	QDir m_usermetricsDirectory;

	QDir m_tempDirectory;

	QString m_aaExec;
};

}

#endif /* USERMETRICSSERVICE_QPROCESSEXECUTOR_H_ */
