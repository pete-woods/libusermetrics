/*
 * Service.cpp
 *
 *  Created on: 2 Mar 2014
 *      Author: pete
 */

#include <infographicservice/InfographicServiceAdaptor.h>
#include <infographicservice/Service.h>
#include <libusermetricscommon/DBusPaths.h>

#include <QFile>
#include <pwd.h>
#include <cstdio>

using namespace InfographicService;
using namespace UserMetricsCommon;

Service::Service(const QDir &directory, const QDBusConnection &systemConnection) :
		m_directory(directory), m_connection(systemConnection), m_adaptor(
				new InfographicsAdaptor(this)), m_hash(QCryptographicHash::Sha1) {

	m_directory.mkpath("tmp");
	m_tempFile.setFileName(
			QDir(m_directory.filePath("tmp")).filePath("tmp.svg"));

	if (!m_connection.registerObject(DBusPaths::INFOGRAPHIC_DBUS_PATH, this)) {
		throw std::logic_error(
				"Unable to register Infographics object on DBus");
	}
	if (!m_connection.registerService(DBusPaths::INFOGRAPHIC_DBUS_NAME)) {
		throw std::logic_error(
				"Unable to register Infographics service on DBus");
	}
}

Service::~Service() {
	m_connection.unregisterService(DBusPaths::INFOGRAPHIC_DBUS_NAME);
	m_connection.unregisterObject(DBusPaths::INFOGRAPHIC_DBUS_PATH);
}

unsigned int Service::uid() {
	if (!calledFromDBus()) {
		return 0;
	}

	return m_connection.interface()->serviceUid(message().service());
}

QDir Service::userDirectory() {
	return m_directory.filePath(QString::number(uid()));
}

void Service::clear() {
	userDirectory().removeRecursively();
}

void Service::update(const QString &visualizer, const QStringList &sources,
		const QString &filePath) {

	m_hash.reset();
	m_hash.addData(visualizer.toUtf8());
	for (const QString &source : sources) {
		m_hash.addData(source.toUtf8());
	}
	QString destinationName(m_hash.result().toHex());
	destinationName.append(".svg");

	QDir usersDirectory(userDirectory());
	QDir infographicDirectory(usersDirectory.filePath(visualizer));
	QString destination(infographicDirectory.filePath(destinationName));

	QFile file(filePath);
	QByteArray ba;
	if (file.open(QIODevice::ReadOnly)) {
		ba = file.readAll();
		file.close();
	} else {
		qWarning() << "Failed to open file" << filePath;
		return;
	}

	if (ba.isEmpty()) {
		return;
	}

	usersDirectory.mkpath(visualizer);
	if (m_tempFile.open(QIODevice::WriteOnly)) {
		m_tempFile.write(ba);
		m_tempFile.close();
	} else {
		qWarning() << "Failed to write file" << m_tempFile.fileName();
	}

	int result = std::rename(m_tempFile.fileName().toUtf8().constData(),
				destination.toUtf8().constData());
	if (result == -1) {
		qWarning() << "Failed to move result" << m_tempFile.fileName() << "to destination"
				<< destination;
	}
}
