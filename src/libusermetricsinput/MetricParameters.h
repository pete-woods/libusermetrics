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

#ifndef USERMETRICSINPUT_METRICPARAMETERS_H_
#define USERMETRICSINPUT_METRICPARAMETERS_H_

#include <libusermetricsinput/Metric.h>

#include <QtCore/QScopedPointer>
#include <QtCore/QString>

namespace UserMetricsInput {

class MetricParametersPrivate;

class Q_DECL_EXPORT MetricParameters {
public:

	explicit MetricParameters(const QString &dataSourceId);

	MetricParameters(const MetricParameters &other);

	virtual ~MetricParameters();

	MetricParameters & operator=(const MetricParameters &other);

	bool operator==(const MetricParameters &other) const;

	MetricParameters & formatString(const QString &formatString);

	MetricParameters & emptyDataString(const QString &emptyDataString);

	MetricParameters & textDomain(const QString &textDomain);

	MetricParameters & minimum(double minimum);

	MetricParameters & maximum(double maximum);

	MetricParameters & type(MetricType type);

	const QString & id() const;

	const QString & formatString() const;

	const QString & emptyDataString() const;

	const QString & textDomain() const;

	MetricType type() const;

	const QVariantMap & options() const;

protected:
	QScopedPointer<MetricParametersPrivate> p;
};

}

#endif // USERMETRICSINPUT_METRICPARAMETERS_H_
