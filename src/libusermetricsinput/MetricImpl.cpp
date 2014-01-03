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

#include <stdexcept>

#include <libusermetricsinput/MetricImpl.h>

#include <QtCore/QVariantList>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>

using namespace std;
using namespace UserMetricsInput;

MetricImpl::MetricImpl(const QDir &metricDirectory,
		const MetricParameters &parameters, Factory::Ptr factory,
		QObject *parent) :
		Metric(parent), m_metricDirectory(metricDirectory), m_parameters(
				parameters), m_factory(factory) {
}

MetricImpl::~MetricImpl() {
}

void MetricImpl::setSelf(QSharedPointer<MetricImpl> self) {
	m_self = self;
}

void MetricImpl::setParameters(const MetricParameters &parameters) {
	m_parameters = parameters;
}

MetricUpdatePtr MetricImpl::update(const QString &username) {
	Q_UNUSED(username);
	loadData();
	return m_factory->newMetricUpdate(m_self);
}

void MetricImpl::update(double value, const QString &username) {
	Q_UNUSED(username);
	loadData();
	update(QVariantList() << value);
}

void MetricImpl::increment(double amount, const QString &username) {
	Q_UNUSED(username);
	loadData();

	QVariantList data;

	QDate currentDate(m_factory->currentDate());
	if (m_lastUpdated == currentDate && !m_currentData.empty()) {
		double value(m_currentData.first().toDouble());
		value += amount;
		data << value;
	} else {
		data << amount;
	}

	update(data);
}

void MetricImpl::update(const QVariantList &data) {
	QDate currentDate(m_factory->currentDate());
	int daysSinceLastUpdate(m_lastUpdated.daysTo(currentDate));

	QVariantList newData(data);

	// if we are in this situation we do nothing
	// new: |4|5|6|7|8|9|0|
	// old:     |1|2|3|
	// res: |4|5|6|7|8|9|0|
	if (daysSinceLastUpdate + m_currentData.size() > newData.size()) {
		if (daysSinceLastUpdate < newData.size()) {
			// if we are in this situation - we need the
			// protruding data from old
			// new: |6|7|8|9|0|
			// old:     |1|2|3|4|5|
			// res: |6|7|8|9|0|4|5|
			auto oldDataIt(m_currentData.constBegin());
			// wind forward until the data we want
			for (int i(daysSinceLastUpdate); i < newData.size(); ++i) {
				++oldDataIt;
			}
			// append the rest of the data
			for (; oldDataIt != m_currentData.constEnd(); ++oldDataIt) {
				newData.append(*oldDataIt);
			}
		} else {
			// we are in this situation - there is a gap
			// and we want the whole of the old data appending
			// new: |6|7|8|9|0|
			// old:             |1|2|3|4|5|
			// res: |6|7|8|9|0|n|1|2|3|4|5|
			const int daysToPad(daysSinceLastUpdate - newData.size());
			// pad the data will null variants
			for (int i(0); i < daysToPad; ++i) {
				newData.append(QVariant(""));
			}
			// append the whole of the old data
			newData.append(m_currentData);
		}
	}

	while (newData.size() > 62) {
		newData.pop_back();
	}

	m_lastUpdated = currentDate;
	m_currentData = newData;
	scaleData();
	writeData();
}

void MetricImpl::scaleData() {
	m_scaledData = m_currentData;

	const QVariantMap &options(m_parameters.options());

	bool hasMinimum(options.contains("minimum"));
	bool hasMaximum(options.contains("maximum"));

	double min(numeric_limits<double>::max());
	double max(numeric_limits<double>::min());

	if (hasMinimum) {
		min = options["minimum"].toDouble();
	}
	if (hasMaximum) {
		max = options["maximum"].toDouble();
	}

	// if we need to find either the max or the min
	if (!hasMinimum || !hasMaximum) {
		for (QVariant &variant : m_scaledData) {
			if (variant.type() == QVariant::String) {
				variant = QVariant();
			} else if (variant.type() == QVariant::Double) {
				double value(variant.toDouble());
				if (!hasMinimum && value < min) {
					min = value;
				}
				if (!hasMaximum && value > max) {
					max = value;
				}
			}
		}
	}

	QVariant head;
	if (m_scaledData.isEmpty()) {
		head = QVariant();
	} else {
		head = m_scaledData.first();
	}

	for (QVariant &variant : m_scaledData) {
		if (variant.type() == QVariant::Double) {
			if (min != max) {
				double value(variant.toDouble());
				if (hasMaximum && value > max) {
					value = max;
				}
				if (hasMinimum && value < min) {
					value = min;
				}
				variant = (value - min) / (max - min);
			} else {
				variant = 0.5;
			}
		}
	}
}

QString MetricImpl::buildJsonFile() {
	return m_metricDirectory.filePath(m_parameters.id() + ".json");
}

void MetricImpl::loadData() {
	if (m_parameters.type() == MetricType::SYSTEM) {
//		qDebug() << "we'd like to do something different with";
	}

	QVariantList existingData;

	QFile jsonFile(buildJsonFile());
	if (jsonFile.open(QIODevice::ReadOnly)) {
		QJsonParseError error;
		QJsonDocument document(
				QJsonDocument::fromJson(jsonFile.readAll(), &error));
		jsonFile.close();
		QVariantMap map(document.toVariant().toMap());
		// There's no point reading the rest of the information.
		// We'll just overwrite it anyway.
		m_currentData = map["data"].toList();
		m_lastUpdated = map["lastUpdated"].toDate();
	}
}

void MetricImpl::writeData() {
	QVariantMap root;
	root["lastUpdated"] = m_lastUpdated;
	root["data"] = m_currentData;
	root["scaledData"] = m_scaledData;
	root["id"] = m_parameters.id();
	root["formatString"] = m_parameters.formatString();
	root["emptyDataString"] = m_parameters.emptyDataString();
	root["textDomain"] = m_parameters.textDomain();
	root["options"] = m_parameters.options();

	QJsonDocument document(QJsonDocument::fromVariant(root));

	QFile jsonFile(buildJsonFile());
	if (jsonFile.open(QIODevice::WriteOnly)) {
		jsonFile.write(document.toJson());
		jsonFile.close();
	}
}
