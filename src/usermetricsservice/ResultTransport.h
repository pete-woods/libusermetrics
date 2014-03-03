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

#ifndef USERMETRICSSERVICE_RESULTTRANSPORT_H_
#define USERMETRICSSERVICE_RESULTTRANSPORT_H_

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

namespace UserMetricsService {

class ResultTransport {
public:
	typedef QSharedPointer<ResultTransport> Ptr;

	ResultTransport();

	virtual ~ResultTransport();

	virtual void send(const QString &id, const QStringList &arguments,
			const QByteArray &ba) = 0;
};

}

#endif /* USERMETRICSSERVICE_RESULTTRANSPORT_H_ */