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

#ifndef USERMETRICSOUTPUT_DATEFACTORY_H_
#define USERMETRICSOUTPUT_DATEFACTORY_H_

#include <QtCore/QDate>

namespace UserMetricsOutput {

class DateFactory: public QObject {
public:
	DateFactory();

	virtual ~DateFactory();

	virtual QDate currentDate() const = 0;
};

}

#endif // USERMETRICSOUTPUT_DATEFACTORY_H_
