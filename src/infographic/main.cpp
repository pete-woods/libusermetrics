/*
 * Copyright (C) 2013 Canonical, Ltd.
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

#include <infographic/Infographic.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>

using namespace std;
using namespace UserMetricsInfographic;

int main(int argc, char *argv[]) {
	QCoreApplication application(argc, argv);

	if (argc != 3) {
		qWarning() << "Usage: " << argv[0] << "INPUT_FILE OUTPUT_FILE";
		return 1;
	}

	QFile inputFile(QString::fromUtf8(argv[1]));
	QFile outputFile(QString::fromUtf8(argv[2]));

	inputFile.open(QIODevice::ReadOnly);
	outputFile.open(QIODevice::WriteOnly);

	Infographic infographic(inputFile, outputFile);

	inputFile.close();
	outputFile.close();

	return 0;
}
