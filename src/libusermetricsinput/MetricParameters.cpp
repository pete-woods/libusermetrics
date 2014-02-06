/*
 * MetricParametersy.cpp
 *
 *  Created on: 11 Dec 2013
 *      Author: pete
 */

#include <libusermetricsinput/MetricParameters.h>

#include <QVariantMap>

namespace UserMetricsInput {

class MetricParametersPrivate {
	friend class MetricManagerImpl;
	friend class MetricParameters;

public:
	explicit MetricParametersPrivate(const QString &dataSourceId) :
			m_dataSourceId(dataSourceId), m_type(MetricType::USER) {
	}

	explicit MetricParametersPrivate(const MetricParametersPrivate &other) :
			m_dataSourceId(other.m_dataSourceId), m_formatString(
					other.m_formatString), m_emptyDataString(
					other.m_emptyDataString), m_textDomain(other.m_textDomain), m_type(
					other.m_type), m_options(other.m_options) {
	}

	~MetricParametersPrivate() {
	}

protected:
	QString m_dataSourceId;

	QString m_formatString;

	QString m_emptyDataString;

	QString m_textDomain;

	MetricType m_type;

	QVariantMap m_options;
};

MetricParameters::MetricParameters(const QString &dataSourceId) :
		p(new MetricParametersPrivate(dataSourceId)) {
}

MetricParameters::MetricParameters(const MetricParameters &other) :
		p(new MetricParametersPrivate(*other.p)) {
}

MetricParameters::~MetricParameters() {
}

MetricParameters & MetricParameters::operator=(const MetricParameters &other) {
	p->m_dataSourceId = other.p->m_dataSourceId;
	p->m_formatString = other.p->m_formatString;
	p->m_emptyDataString = other.p->m_emptyDataString;
	p->m_textDomain = other.p->m_textDomain;
	p->m_type = other.p->m_type;
	p->m_options = other.p->m_options;
	return *this;
}

bool MetricParameters::operator==(const MetricParameters &other) const {
	return p->m_dataSourceId == other.p->m_dataSourceId
			&& p->m_formatString == other.p->m_formatString
			&& p->m_emptyDataString == other.p->m_emptyDataString
			&& p->m_textDomain == other.p->m_textDomain
			&& p->m_type == other.p->m_type
			&& p->m_options == other.p->m_options;
}

MetricParameters & MetricParameters::formatString(const QString &formatString) {
	p->m_formatString = formatString;
	return *this;
}

MetricParameters & MetricParameters::emptyDataString(
		const QString &emptyDataString) {
	p->m_emptyDataString = emptyDataString;
	return *this;
}

MetricParameters & MetricParameters::textDomain(const QString &textDomain) {
	p->m_textDomain = textDomain;
	return *this;
}

MetricParameters & MetricParameters::minimum(double minimum) {
	p->m_options["minimum"] = minimum;
	return *this;
}

MetricParameters & MetricParameters::maximum(double maximum) {
	p->m_options["maximum"] = maximum;
	return *this;
}

MetricParameters & MetricParameters::type(MetricType type) {
	p->m_type = type;
	return *this;
}

const QString & MetricParameters::id() const {
	return p->m_dataSourceId;
}

const QString & MetricParameters::formatString() const {
	return p->m_formatString;
}

const QString & MetricParameters::emptyDataString() const {
	return p->m_emptyDataString;
}

const QString & MetricParameters::textDomain() const {
	return p->m_textDomain;
}

MetricType MetricParameters::type() const {
	return p->m_type;
}

const QVariantMap & MetricParameters::options() const {
	return p->m_options;
}

}
