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

#ifndef USERMETRICSSERVICE_RESULTTRANSPORTIMPL_H_
#define USERMETRICSSERVICE_RESULTTRANSPORTIMPL_H_

#include <libusermetricscommon/InfographicsInterface.h>
#include <usermetricsservice/ResultTransport.h>

namespace UserMetricsService {

class ResultTransportImpl: public ResultTransport {
public:
	ResultTransportImpl(
			QSharedPointer<ComCanonicalInfographicsInterface> infographicService);

	virtual ~ResultTransportImpl();

	void clear();

	void send(const QString &id, const QStringList &arguments,
			const QByteArray &ba);

protected:
	QSharedPointer<ComCanonicalInfographicsInterface> m_infographicService;
};

}

#endif /* USERMETRICSSERVICE_RESULTTRANSPORTIMPL_H_ */
