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

#define _USE_MATH_DEFINES

#include <cmath>
#include <QDate>
#include <QDebug>
#include <QIODevice>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QStringList>
#include <QVariantList>
#include <QXmlStreamWriter>

using namespace UserMetricsInfographic;

static const double CENTER_CIRCLE_RADIUS = 345.0;
static const double DATA_CIRCLE_RADIUS = CENTER_CIRCLE_RADIUS / 1.8;
static const double DOT_DISTANCE = 0.8 * CENTER_CIRCLE_RADIUS;
static const double SVG_SIZE = 2.0 * CENTER_CIRCLE_RADIUS
		+ 2.0 * DATA_CIRCLE_RADIUS;
static const double CENTER_COORD = 0.5 * SVG_SIZE;
static const int LINE_WIDTH(20);

static const QRegularExpression WHITESPACE("\\s");

Infographic::Infographic(QIODevice &input, QIODevice &output) {
	readJson(input);
	splitDataIntoMonths();
	writeSvg(output);
}

Infographic::~Infographic() {
}

static inline double xComponent(double radius, int count, int total) {
	return CENTER_COORD + radius * sin((2.0 * M_PI * count) / total);
}

static inline double yComponent(double radius, int count, int total) {
	return CENTER_COORD - radius * cos((2.0 * M_PI * count) / total);
}

void Infographic::splitDataIntoMonth(QVariantList &month, const int dayOfMonth,
		const int daysInMonth, QVariantList::const_iterator& index,
		const QVariantList::const_iterator& end) {

	// Copy a number of data entries equal to the day of the month it is
	for (int i(0); i < dayOfMonth; ++i) {
		if (index == end) {
			// when we run out of data, pad the remaining days to the
			// start of the month
			month.prepend(QVariant());
		} else {
			// pop data from the from of the source
			month.prepend(*index);
			++index;
		}
	}

	// Now fill the end of the month with empty data
	while (month.size() < daysInMonth) {
		month.append(QVariant());
	}
}

void Infographic::splitDataIntoMonths() {
	QDate currentDate(QDate::currentDate());
	QDate secondMonthDate(currentDate.addMonths(-1));

	int valuesToCopyForFirstMonth(0);
	int valuesToCopyForSecondMonth(0);

	if (currentDate.year() == m_lastUpdated.year()
			&& currentDate.month() == m_lastUpdated.month()) {
		// If the data is for the first month
		valuesToCopyForFirstMonth = m_lastUpdated.day();
		valuesToCopyForSecondMonth = secondMonthDate.daysInMonth();
	} else if (secondMonthDate.year() == m_lastUpdated.year()
			&& secondMonthDate.month() == m_lastUpdated.month()) {
		// If the data is for the second month
		valuesToCopyForSecondMonth = m_lastUpdated.day();
	} else {
		// the data is out of date
	}

	QVariantList::const_iterator dataIndex(m_data.begin());
	QVariantList::const_iterator end(m_data.end());

	splitDataIntoMonth(m_firstMonth, valuesToCopyForFirstMonth,
			currentDate.daysInMonth(), dataIndex, end);

	splitDataIntoMonth(m_secondMonth, valuesToCopyForSecondMonth,
			secondMonthDate.daysInMonth(), dataIndex, end);

	m_currentDay = valuesToCopyForFirstMonth - 1;
}

void Infographic::readJson(QIODevice &input) {
	QJsonDocument document(QJsonDocument::fromJson(input.readAll()));
	QVariantMap map(document.toVariant().toMap());

	m_label = map["label"].toString();
	m_data = map["scaledData"].toList();
	m_lastUpdated = map["lastUpdated"].toDate();
}

void Infographic::writeCircle(double x, double y, double r,
		const QString &color, double opacity, const QString &id,
		const QString &clazz, QXmlStreamWriter &stream) {
	stream.writeStartElement("circle");
	stream.writeAttribute("id", id);
	stream.writeAttribute("class", clazz);
	stream.writeAttribute("stroke", "none");
	stream.writeAttribute("opacity", QString::number(opacity));
	stream.writeAttribute("fill", color);
	stream.writeAttribute("cx", QString::number(x));
	stream.writeAttribute("cy", QString::number(y));
	stream.writeAttribute("r", QString::number(r));
	stream.writeEndElement();
}

void Infographic::writeRing(double x, double y, double r, const QString &color,
		double opacity, const QString &id, const QString &clazz,
		QXmlStreamWriter &stream) {
	stream.writeStartElement("circle");
	stream.writeAttribute("id", id);
	stream.writeAttribute("class", clazz);
	stream.writeAttribute("stroke", color);
	stream.writeAttribute("stroke-width", "1");
	stream.writeAttribute("opacity", QString::number(opacity));
	stream.writeAttribute("fill", "none");
	stream.writeAttribute("cx", QString::number(x));
	stream.writeAttribute("cy", QString::number(y));
	stream.writeAttribute("r", QString::number(r));
	stream.writeEndElement();
}

void Infographic::writeDots(int currentDay, int days, const QString &color,
		QXmlStreamWriter &stream) {

	uint dotId(0);

	for (int day(0); day < currentDay; ++day) {
		writeCircle(xComponent(DOT_DISTANCE, day, days),
				yComponent(DOT_DISTANCE, day, days), 8.0, color, 0.4,
				QString("day-alt-%1").arg(++dotId), "day-alt-past", stream);
	}

	{
		double x(xComponent(DOT_DISTANCE, currentDay, days));
		double y(yComponent(DOT_DISTANCE, currentDay, days));

		stream.writeStartElement("image");
		stream.writeAttribute("id",
				QString("day-alt-%1").arg(QString("dot-%1").arg(++dotId)));
		stream.writeAttribute("class", "day-alt-current");
		stream.writeAttribute("opacity", "0.4");
		stream.writeAttribute("x", QString::number(x - 20.0));
		stream.writeAttribute("y", QString::number(y - 20.0));
		stream.writeAttribute("width", "40px");
		stream.writeAttribute("height", "40px");
		stream.writeAttribute("transform",
				QString("rotate(%1 %2 %3)").arg((360.0 * currentDay) / days).arg(
						x).arg(y));
		stream.writeAttribute("xlink:href",
				"/usr/share/unity8/Greeter/graphics/dot_pointer.png");
		stream.writeEndElement();
	}

	for (int day(currentDay + 1); day < days; ++day) {
		writeRing(xComponent(DOT_DISTANCE, day, days),
				yComponent(DOT_DISTANCE, day, days), 7.0, color, 0.4,
				QString("day-alt-%1").arg(++dotId), "day-alt-future", stream);
	}
}

void Infographic::writeMonth(const QVariantList &month, const QString &color,
		const QString &id, const QString &clazz, QXmlStreamWriter &stream) {
	int day(0);
	int days(month.size());
	for (const QVariant &value : month) {
		if (!value.isNull()) {
			writeCircle(xComponent(CENTER_CIRCLE_RADIUS, day, days),
					yComponent(CENTER_CIRCLE_RADIUS, day, days),
					DATA_CIRCLE_RADIUS * value.toDouble(), color, 0.3,
					id.arg(day), clazz, stream);
		}
		++day;
	}
}

void Infographic::writeLabel(QXmlStreamWriter &stream) {
	m_label.remove("<b>", Qt::CaseInsensitive);
	m_label.remove("</b>", Qt::CaseInsensitive);

	QStringList label;

	QStringList words(m_label.split(WHITESPACE));
	int spaceLeft = LINE_WIDTH;
	int lineLength(0);
	int lineStart(0);
	for (const QString &word : words) {
		lineLength += word.length() + 1;
		if (word.length() + 1 > spaceLeft) {
			label << m_label.mid(lineStart, lineLength);
			lineStart += lineLength;
			lineLength = 0;

			spaceLeft = LINE_WIDTH - word.length();
		} else {
			spaceLeft = spaceLeft - (word.length() + 1);
		}
	}
	if (lineLength > 0) {
		label << m_label.mid(lineStart, lineLength);
	}

	int lines(label.size());
	int yOffset(CENTER_COORD - 15.0 * lines);
	int lineNumber(0);
	for (const QString &line : label) {
		stream.writeStartElement("text");
		stream.writeAttribute("id", QString("label-line-%1").arg(lineNumber));
		stream.writeAttribute("class", "label");
		stream.writeAttribute("opacity", "0.6");
		stream.writeAttribute("x", QString::number(CENTER_COORD));
		stream.writeAttribute("y",
				QString::number(yOffset + 50.0 * lineNumber));
		stream.writeAttribute("fill", "#ffffff");
		stream.writeAttribute("text-anchor", "middle");
		stream.writeAttribute("font-family", "Ubuntu");
		stream.writeAttribute("font-size", "40");
		stream.writeCharacters(line);
		stream.writeEndElement();
		++lineNumber;
	}
}

void Infographic::writeSvg(QIODevice &output) {
	QXmlStreamWriter stream(&output);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("svg");
	stream.writeAttribute("xmlns", "http://www.w3.org/2000/svg");
	stream.writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
	stream.writeAttribute("width", QString::number(ceil(SVG_SIZE)));
	stream.writeAttribute("height", QString::number(ceil(SVG_SIZE)));

	writeMonth(m_secondMonth, "#ff9900", "day-past-%1", "day-past", stream);
	writeMonth(m_firstMonth, "#e54c19", "day-present-%1", "day-present",
			stream);

	{
		// 689 x 691
		stream.writeStartElement("image");
		stream.writeAttribute("x",
				QString::number(ceil(CENTER_COORD - 689.0 / 2)));
		stream.writeAttribute("y",
				QString::number(ceil(CENTER_COORD - 691.0 / 2)));
		stream.writeAttribute("width", "689px");
		stream.writeAttribute("height", "691px");
		stream.writeAttribute("xlink:href",
				"/usr/share/unity8/Greeter/graphics/infographic_circle_back.png");
		stream.writeEndElement();
	}

	writeLabel(stream);

	writeDots(m_currentDay, m_firstMonth.size(), "#ffffff", stream);

	stream.writeEndElement();
	stream.writeEndDocument();
}
