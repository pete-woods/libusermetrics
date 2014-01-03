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

#ifndef USERMETRICSINPUT_METRICIMPL_H_
#define USERMETRICSINPUT_METRICIMPL_H_

#include <libusermetricsinput/Factory.h>
#include <libusermetricsinput/MetricImpl.h>
#include <libusermetricsinput/MetricParameters.h>

#include <QtCore/QDir>
#include <QtCore/QDate>
#include <QtCore/QString>

namespace UserMetricsInput {

class MetricImpl: public Metric {
public:
	typedef QSharedPointer<MetricImpl> Ptr;

	explicit MetricImpl(const QDir &metriccDirectory,
			const MetricParameters &parameters,
			Factory::Ptr factory,
			QObject *parent = 0);

	virtual ~MetricImpl();

	virtual MetricUpdatePtr update(const QString &username = "");

	virtual void update(double value, const QString &username = "");

	virtual void increment(double amount = 1.0f, const QString &username = "");

	void setParameters(const MetricParameters &parameters);

	void setSelf(QSharedPointer<MetricImpl> self);

	void update(const QVariantList &data);

protected:
	void scaleData();

	void loadData();

	void writeData();

	QString buildJsonFile();

	QDir m_metricDirectory;

	MetricParameters m_parameters;

	Factory::Ptr m_factory;

	QWeakPointer<MetricImpl> m_self;

	QVariantList m_currentData;

	QVariantList m_scaledData;

	QDate m_lastUpdated;
};

}

#endif // USERMETRICSINPUT_METRICIMPL_H_
