/*
 * Service.cpp
 *
 *  Created on: 2 Mar 2014
 *      Author: pete
 */

#include <infographicservice/InfographicServiceAdaptor.h>
#include <infographicservice/Service.h>

#include <QFile>
#include <pwd.h>

using namespace InfographicService;

Service::Service(const QDir &directory, const QDBusConnection &systemConnection) :
		m_connection(systemConnection), m_adaptor(new InfographicsAdaptor(this)) {

	if (!m_connection.registerObject("/com/canonical/Infographics", this)) {
		throw std::logic_error(
				"Unable to register Infographics object on DBus");
	}
	if (!m_connection.registerService("com.canonical.Infographics")) {
		throw std::logic_error(
				"Unable to register Infographics service on DBus");
	}
}

Service::~Service() {
	m_connection.unregisterService("com.canonical.Infographics");
	m_connection.unregisterObject("/com/canonical/Infographics");
}

unsigned int Service::uid() {
	if (!calledFromDBus()) {
		return 0;
	}

	return m_connection.interface()->serviceUid(message().service());
}

void Service::clear() {
	qDebug() << "clear" << uid();
}

void Service::update(const QString &visualizer, const QStringList &sources,
		const QString &filePath) {

	qDebug() << "update" << visualizer << sources << filePath << uid();

	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)) {
		qDebug() << "read file bytes" << file.readAll().size();
		file.close();
	}

	qDebug() << "update complete";
}
