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

#include <libusermetricsinput/MetricImpl.h>
#include <libusermetricsinput/MetricManagerImpl.h>

#include <stdexcept>
#include <QDebug>

using namespace std;
using namespace UserMetricsInput;

MetricManagerImpl::MetricManagerImpl(Factory::Ptr factory,
		const QDir &cacheDirectory, const QString &applicationId,
		QObject *parent) :
		MetricManager(parent), m_factory(factory) {
	if (applicationId.isEmpty()) {
		throw logic_error("Invalid application ID");
	}
	QDir applicationDirectory(cacheDirectory.filePath(applicationId));
	if (!applicationDirectory.mkpath("usermetrics")) {
		throw logic_error("Cannot write to cache directory");
	}
	m_metricDirectory = applicationDirectory.filePath("usermetrics");
}

MetricManagerImpl::~MetricManagerImpl() {
}

MetricPtr MetricManagerImpl::add(const QString &dataSourceId,
		const QString &formatString, const QString &emptyDataString,
		const QString &textDomain) {
	return add(
			MetricParameters(dataSourceId).formatString(formatString).emptyDataString(
					emptyDataString).textDomain(textDomain));
}

MetricPtr MetricManagerImpl::add(const MetricParameters &parameters) {
	const QString &dataSourceId(parameters.id());
	auto metric(m_metrics.find(dataSourceId));
	if (metric == m_metrics.end()) {
		MetricPtr newMetric(
				m_factory->newMetric(m_metricDirectory, parameters, m_factory));
		metric = m_metrics.insert(dataSourceId, newMetric);
	} else {
		MetricImpl::Ptr existingMetric(metric->dynamicCast<MetricImpl>());
		if(existingMetric) {
			existingMetric->setParameters(parameters);
		}
	}
	return *metric;
}
