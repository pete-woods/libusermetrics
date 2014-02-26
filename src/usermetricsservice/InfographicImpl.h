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

#ifndef USERMETRICSSERVICE_INFOGRAPHICIMPL_H_
#define USERMETRICSSERVICE_INFOGRAPHICIMPL_H_

#include <usermetricsservice/Infographic.h>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace UserMetricsService {

class InfographicImpl: public Infographic {
public:
	InfographicImpl(const QVariant &config);

	virtual ~InfographicImpl();

	bool isValid() const;

	void sourcesChanged(const QMultiMap<QString, QString> &changedSources,
			const QMultiMap<QString, QString> &allSources);

protected:
	void iterate(const QMultiMap<QString, QString> &changedSources);

	void aggregate(const QMultiMap<QString, QString> &changedSources,
			const QMultiMap<QString, QString> &allSources);

	void execute(const QStringList &args);

	QStringList match(const QMultiMap<QString, QString>& changedSources);

	QString m_exec;

	Type m_type;

	QMap<QString, QStringList> m_rules;

	int m_ruleCount;

};

}

#endif /* USERMETRICSSERVICE_INFOGRAPHICIMPL_H_ */
