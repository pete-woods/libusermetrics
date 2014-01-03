/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#ifndef LIBUSERMETRICSINPUT_TEST_MOCKS_H_
#define LIBUSERMETRICSINPUT_TEST_MOCKS_H_

#include <libusermetricsinput/Metric.h>
#include <libusermetricsinput/Factory.h>

#include <QtCore/QDate>
#include <gmock/gmock.h>

namespace UserMetricsInput {
namespace Mocks {

class MockFactory: public Factory {
public:
	MOCK_CONST_METHOD0(currentDate, QDate());

	MOCK_METHOD3(newMetric, MetricPtr(const QDir &,
					const MetricParameters &, Factory::Ptr));
};

class MockMetric: public Metric {
public:
	MOCK_METHOD1(update, MetricUpdatePtr(const QString &));

	MOCK_METHOD2(update, void(double, const QString &));

	MOCK_METHOD2(increment, void(double, const QString &));
};

}
}

#endif /* LIBUSERMETRICSINPUT_TEST_MOCKS_H_ */
