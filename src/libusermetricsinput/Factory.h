/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#ifndef USERMETRICSINPUT_FACTORY_H_
#define USERMETRICSINPUT_FACTORY_H_

#include <libusermetricsinput/MetricParameters.h>

#include <QtCore/QDir>
#include <QtCore/QSharedPointer>

namespace UserMetricsInput {

class Factory {
public:
	typedef QSharedPointer<Factory> Ptr;

	Factory();

	virtual ~Factory();

	virtual MetricPtr newMetric(const QDir &metricsDirectory,
			const MetricParameters &parameters, Factory::Ptr factory);

	virtual MetricUpdatePtr newMetricUpdate(MetricPtr metric);

	virtual QDate currentDate() const;
};

}

#endif /* USERMETRICSINPUT_FACTORY_H_ */
