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
#include <usermetricsservice/Service.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTemporaryFile>

using namespace UserMetricsService;

InfographicImpl::InfographicImpl(const QFile &path, bool click,
		Executor::Ptr executor, ResultTransport::Ptr resultTransport) :
		m_path(path.fileName()), m_click(click), m_executor(executor), m_resultTransport(
				resultTransport), m_type(Type::INVALID), m_ruleCount(0) {

	if (!m_path.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open path:" << m_path.fileName();
		return;
	}

	QByteArray ba(m_path.readAll());
	m_path.close();

	QJsonParseError error;
	QJsonDocument document(QJsonDocument::fromJson(ba, &error));
	if (error.error != QJsonParseError::NoError) {
		qWarning() << "Failed to parse infographic JSON document:"
				<< m_path.fileName() << error.errorString();
		return;
	}

	if (document.isNull() || document.isEmpty()) {
		return;
	}

	QFileInfo fileInfo(m_path);
	m_id = fileInfo.completeBaseName();

	QVariantMap map(document.toVariant().toMap());

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
		auto list = m_rules.insert(i.key(), QList<QRegularExpression>());
		for (const QString &rule : rules) {
			*list << QRegularExpression(rule);
		}
		m_ruleCount += rules.size();
	}
}

InfographicImpl::~InfographicImpl() {
}

bool InfographicImpl::isValid() const {
	return QFile::exists(m_exec) && m_type != Type::INVALID;
}

Infographic::Type InfographicImpl::type() const {
	return m_type;
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

	QMapIterator<QString, QList<QRegularExpression>> iter(m_rules);
	while (iter.hasNext()) {
		iter.next();
		const QString& sourceName(iter.key());
		QStringList changedFiles;
		if (sourceName == "*") {
			changedFiles = changedSources.values();
		} else if (changedSources.contains(sourceName)) {
			changedFiles = changedSources.values(sourceName);
		}

		const QList<QRegularExpression> &watchedFiles(iter.value());
		for (const QRegularExpression &watchedFile : watchedFiles) {
			for (const QString &changedFile : changedFiles) {
				QString changedFileName(QFileInfo(changedFile).fileName());
				if (watchedFile.match(changedFileName).hasMatch()) {
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

void InfographicImpl::execute(const QStringList &arguments) {
	QByteArray ba(
			m_executor->execute(m_exec, m_click ? m_id : QString(), arguments));

	if (ba.isEmpty()) {
		qWarning() << "No data provided by infographic" << m_exec;
		return;
	}

	m_resultTransport->send(m_id, arguments, ba);
}
