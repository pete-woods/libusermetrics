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

#include <libusermetricsinput/Factory.h>
#include <libusermetricsinput/MetricImpl.h>
#include <libusermetricsinput/MetricUpdateImpl.h>

namespace UserMetricsInput {

Factory::Factory() {
}

Factory::~Factory() {
}

MetricPtr Factory::newMetric(const QDir &metricsDirectory,
		const MetricParameters &parameters, Factory::Ptr factory) {
	QSharedPointer<MetricImpl> metric(
			new MetricImpl(metricsDirectory, parameters, factory));
	metric->setSelf(metric);
	return metric;
}

MetricUpdatePtr Factory::newMetricUpdate(MetricPtr metric) {
	return MetricUpdatePtr(
			new MetricUpdateImpl(metric.staticCast<MetricImpl>()));
}

QDate Factory::currentDate() const {
	return QDate::currentDate();
}

}
