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

#include <usermetricsservice/Executor.h>
#include <usermetricsservice/Infographic.h>
#include <usermetricsservice/ResultTransport.h>

#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace UserMetricsService {

class InfographicImpl: public Infographic {
public:
	InfographicImpl(const QFile &path, bool click, Executor::Ptr executor,
			ResultTransport::Ptr resultTransport);

	virtual ~InfographicImpl();

	bool isValid() const;

	Type type() const;

	void sourcesChanged(const QMultiMap<QString, QString> &changedSources,
			const QMultiMap<QString, QString> &allSources);

protected:
	void iterate(const QMultiMap<QString, QString> &changedSources);

	void aggregate(const QMultiMap<QString, QString> &changedSources,
			const QMultiMap<QString, QString> &allSources);

	void execute(const QStringList &args);

	QStringList match(const QMultiMap<QString, QString>& changedSources);

	QFile m_path;

	bool m_click;

	Executor::Ptr m_executor;

	ResultTransport::Ptr m_resultTransport;

	QString m_exec;

	Type m_type;

	QMap<QString, QList<QRegularExpression>> m_rules;

	int m_ruleCount;

	QString m_id;

};

}

#endif /* USERMETRICSSERVICE_INFOGRAPHICIMPL_H_ */
