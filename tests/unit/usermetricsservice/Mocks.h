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

#ifndef USERMETRICSSERVICE_TEST_MOCKS_H_
#define USERMETRICSSERVICE_TEST_MOCKS_H_

#include <usermetricsservice/Factory.h>

#include <gmock/gmock.h>

namespace UserMetricsService {
namespace Mocks {

class MockFactory: public Factory {
public:
	MOCK_METHOD0(singletonService, Service::Ptr());

	MOCK_METHOD0(singletonFileUtils, FileUtils::Ptr());

	MOCK_METHOD0(singletonExecutor, Executor::Ptr());

	MOCK_METHOD0(singletonInfographicService, QSharedPointer<ComCanonicalInfographicsInterface>());

	MOCK_METHOD0(singletonResultTransport, ResultTransport::Ptr());

	MOCK_METHOD2(newInfographic, Infographic::Ptr(const QFile &,
					const Service &));

	MOCK_METHOD1(newSourceDirectory, SourceDirectory::Ptr(const QDir &));
};


class MockExecutor: public Executor {
public:

	MOCK_METHOD2(execute, QByteArray(const QString &,
			const QStringList &));
};

class MockResultTransport: public ResultTransport {
public:

	MOCK_METHOD3(send, void(const QString &, const QStringList &,
					const QByteArray &));
};

}
}

#endif /* USERMETRICSSERVICE_TEST_MOCKS_H_ */
