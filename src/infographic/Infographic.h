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

#ifndef INFOGRAPHIC_H_
#define INFOGRAPHIC_H_

#include <QDate>
#include <QVariantList>

class QIODevice;
class QString;
class QXmlStreamWriter;

namespace UserMetricsInfographic {

class Infographic {
public:
	Infographic(QIODevice &input, QIODevice &output);

	virtual ~Infographic();

protected:
	void readJson(QIODevice &input);

	void splitDataIntoMonths();

	void splitDataIntoMonth(QVariantList &month, const int dayOfMonth,
			const int daysInMonth, QVariantList::const_iterator& index,
			const QVariantList::const_iterator& end);

	void writeCircle(double x, double y, double r, const QString &color,
			double opacity, QXmlStreamWriter &stream);

	void writeRing(double x, double y, double r, const QString &color,
			double opacity, QXmlStreamWriter &stream);

	void writeDots(int currentDay, int days, const QString &color,
			QXmlStreamWriter &stream);

	void writeMonth(const QVariantList &month, const QString &color,
			QXmlStreamWriter &stream);

	void writeLabel(QXmlStreamWriter &stream);

	void writeSvg(QIODevice &output);

	QString m_label;

	int m_currentDay;

	QDate m_lastUpdated;

	QVariantList m_data;

	QVariantList m_firstMonth;

	QVariantList m_secondMonth;
};

} /* namespace UserMetricsInfographic */

#endif /* INFOGRAPHIC_H_ */
