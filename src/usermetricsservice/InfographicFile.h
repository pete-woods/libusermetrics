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

#ifndef USERMETRICSSERVICE_INFOGRAPHICFILE_H_
#define USERMETRICSSERVICE_INFOGRAPHICFILE_H_

#include <QSharedPointer>

namespace UserMetricsService {

class InfographicFile {
public:
	typedef QSharedPointer<InfographicFile> Ptr;

	InfographicFile();

	virtual ~InfographicFile();
};

}

#endif /* USERMETRICSSERVICE_INFOGRAPHICFILE_H_ */
