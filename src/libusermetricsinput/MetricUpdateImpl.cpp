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
#include <libusermetricsinput/MetricUpdateImpl.h>

#include <stdexcept>

using namespace std;
using namespace UserMetricsInput;

MetricUpdateImpl::MetricUpdateImpl(QSharedPointer<MetricImpl> metric,
		QObject *parent) :
		MetricUpdate(parent), m_metric(metric) {
}

MetricUpdateImpl::~MetricUpdateImpl() {
	m_metric->update(m_data);
}

void MetricUpdateImpl::addData(double data) {
	m_data << data;
}

void MetricUpdateImpl::addNull() {
	m_data << QVariant();
}
