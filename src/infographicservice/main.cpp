/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#include <infographicservice/Service.h>

#include <QCoreApplication>
#include <QDebug>
#include <csignal>

using namespace InfographicService;

static void exitQt(int sig) {
	Q_UNUSED(sig);
	QCoreApplication::exit(0);
}

int main(int argc, char *argv[]) {
	QCoreApplication application(argc, argv);

	signal(SIGINT, &exitQt);
	signal(SIGTERM, &exitQt);

	try {
		Service service(QDir("/var/lib/usermetrics"),
				QDBusConnection::systemBus());
		return application.exec();
	} catch (std::exception &e) {
		qWarning() << "Infographic Service:" << e.what();
		return 1;
	}
}
