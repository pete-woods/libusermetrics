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

#include <usermetricsservice/InfographicImpl.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

using namespace UserMetricsService;

InfographicImpl::InfographicImpl(const QVariant &config) :
		m_type(Type::INVALID), m_ruleCount(0) {

	QVariantMap map(config.toMap());

	m_exec = map["exec"].toString();

	QString type = map["type"].toString().toLower();
	if (type == "iterate") {
		m_type = Type::ITERATE;
	} else if (type == "aggregate") {
		m_type = Type::AGGREGATE;
	}

	QVariantMap input(map["input"].toMap());
	QMapIterator<QString, QVariant> i(input);
	while (i.hasNext()) {
		i.next();
		QStringList rules(i.value().toStringList());
		m_rules[i.key()] = rules;
		m_ruleCount += rules.size();
	}
}

InfographicImpl::~InfographicImpl() {
}

bool InfographicImpl::isValid() const {
	return QFile::exists(m_exec) && m_type != Type::INVALID;
}

void InfographicImpl::sourcesChanged(
		const QMultiMap<QString, QString> &changedSources,
		const QMultiMap<QString, QString> &allSources) {
	switch (m_type) {
	case Type::INVALID:
		break;
	case Type::ITERATE:
		iterate(changedSources);
		break;
	case Type::AGGREGATE:
		aggregate(changedSources, allSources);
		break;
	}
}

QStringList InfographicImpl::match(
		const QMultiMap<QString, QString>& changedSources) {

	QStringList output;

	QMapIterator<QString, QStringList> iter(m_rules);
	while (iter.hasNext()) {
		iter.next();
		const QString& sourceName(iter.key());
		QStringList changedFiles;
		if (sourceName == "*") {
			changedFiles = changedSources.values();
		} else if (changedSources.contains(sourceName)) {
			changedFiles = changedSources.values(sourceName);
		}

		const QStringList& watchedFiles(iter.value());
		for (const QString& changedFile : changedFiles) {
			QString changedFileName(QFileInfo(changedFile).fileName());
			for (const QString& watchedFile : watchedFiles) {
				QRegularExpression re(watchedFile);
				if (re.match(changedFileName).hasMatch()) {
					output << changedFile;
				}
			}
		}
	}
	return output;
}

void InfographicImpl::iterate(
		const QMultiMap<QString, QString> &changedSources) {

	for (const QString &changedFile : match(changedSources)) {
		execute(QStringList() << changedFile);
	}
}

void InfographicImpl::aggregate(
		const QMultiMap<QString, QString> &changedSources,
		const QMultiMap<QString, QString> &allSources) {

	if (match(changedSources).isEmpty()) {
		return;
	}

	// If we get a match for every rule
	QStringList matches(match(allSources));
	if (m_ruleCount == matches.size()) {
		execute(matches);
	}
}

void InfographicImpl::execute(const QStringList &args) {
	qDebug() << "execute" << m_exec << args;
}
