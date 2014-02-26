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

#ifndef USERMETRICSSERVICE_INFOGRAPHIC_H_
#define USERMETRICSSERVICE_INFOGRAPHIC_H_

#include <QSharedPointer>

namespace UserMetricsService {

class Infographic: public QObject {
Q_OBJECT

public:
	typedef QSharedPointer<Infographic> Ptr;

	enum class Type {
		INVALID, ITERATE, AGGREGATE,
	};

	Infographic();

	virtual ~Infographic();

	virtual bool isValid() const = 0;

public Q_SLOTS:
	virtual void sourcesChanged(
			const QMultiMap<QString, QString> &changedSources,
			const QMultiMap<QString, QString> &allSources) = 0;
};

}

#endif /* USERMETRICSSERVICE_INFOGRAPHIC_H_ */
