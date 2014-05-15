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

#include <stdexcept>

#include <usermetricsservice/InfographicImpl.h>

#include <unit/usermetricsservice/Mocks.h>
#include <testutils/QStringPrinter.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace testing;
using namespace UserMetricsService;
using namespace UserMetricsService::Mocks;

namespace {

class TestInfographicImpl: public Test {
protected:
	void SetUp() {
		executor.reset(new StrictMock<MockExecutor>);

		resultTransport.reset(new StrictMock<MockResultTransport>);
	}

	QSharedPointer<MockExecutor> executor;

	QSharedPointer<MockResultTransport> resultTransport;

	Infographic::Ptr newInfographic(const QString &path) {
		return Infographic::Ptr(
				new InfographicImpl(path, false, executor, resultTransport));
	}
};

TEST_F(TestInfographicImpl, Iterate) {
	Infographic::Ptr infographic(newInfographic (TEST_ITERATE_INFOGRAPHIC));

	EXPECT_TRUE(infographic->isValid());
	EXPECT_EQ(Infographic::Type::ITERATE, infographic->type());

	QMultiMap<QString, QString> changedSources;
	changedSources.insert("apple", "/path/orchard.libusermetrics.json");
	changedSources.insert("apple", "/path/grove.json");
	changedSources.insert("banana", "/path/tree.libusermetrics.json");

	QMultiMap<QString, QString> allSources;

	QByteArray orchardBa("orchard");
	QStringList orchard;
	orchard << "/path/orchard.libusermetrics.json";
	EXPECT_CALL(*executor,
			execute(QString("/bin/echo"), QString(), orchard)).WillOnce(Return(orchardBa));
	EXPECT_CALL(*resultTransport,
			send(QString("test-iterate"), orchard, orchardBa));

	QByteArray treeBa("tree blah");
	QStringList tree;
	tree << "/path/tree.libusermetrics.json";
	EXPECT_CALL(*executor,
			execute(QString("/bin/echo"), QString(), tree)).WillOnce(Return(treeBa));
	EXPECT_CALL(*resultTransport, send(QString("test-iterate"), tree, treeBa));

	infographic->sourcesChanged(changedSources, allSources);
}

TEST_F(TestInfographicImpl, Aggregate) {
	Infographic::Ptr infographic(newInfographic (TEST_AGGREGATE_INFOGRAPHIC));

	EXPECT_TRUE(infographic->isValid());
	EXPECT_EQ(Infographic::Type::AGGREGATE, infographic->type());

	QMultiMap<QString, QString> changedSources;
	QMultiMap<QString, QString> allSources;

	// Only two of the 3 requires sources are present
	allSources.insert("foo", "/path/source-1.libusermetrics.json");
	allSources.insert("foo", "/path/source-2.libusermetrics.json");
	allSources.insert("com.ubuntu.camera",
			"/path/camera-photos.libusermetrics.json");
	allSources.insert("com.ubuntu.camera",
			"/path/camera-videos.libusermetrics.json");
	allSources.insert("bar", "/path/bar.libusermetrics.json");

	changedSources.insert("com.ubuntu.camera",
			"/path/camera-videos.libusermetrics.json");

	// Nothing should happen
	infographic->sourcesChanged(changedSources, allSources);

	// Now all of the required sources are present
	allSources.insert("foo", "/path/source-id.libusermetrics.json");

	QByteArray ba("stuff in the byte array");
	QStringList files;
	files << "/path/camera-photos.libusermetrics.json"
			<< "/path/camera-videos.libusermetrics.json"
			<< "/path/source-id.libusermetrics.json";
	EXPECT_CALL(*executor,
			execute(QString("/bin/cat"), QString(), files)).WillOnce(Return(ba));
	EXPECT_CALL(*resultTransport, send(QString("test-aggregate"), files, ba));

	infographic->sourcesChanged(changedSources, allSources);
}

} // namespace
