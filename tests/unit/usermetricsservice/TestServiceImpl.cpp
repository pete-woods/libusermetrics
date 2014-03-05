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

#include <usermetricsservice/ServiceImpl.h>
#include <usermetricsservice/SourceDirectoryImpl.h>
#include <unit/usermetricsservice/Mocks.h>
#include <testutils/QStringPrinter.h>

#include <QSignalSpy>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>

using namespace std;
using namespace testing;
using namespace UserMetricsCommon;
using namespace UserMetricsService;
using namespace UserMetricsService::Mocks;

MATCHER_P(HasPath, value, ""){ return arg.fileName() == value;}

namespace {

class TestServiceImpl: public Test {
protected:
	void SetUp() {
		qRegisterMetaType<QMultiMap<QString, QString>>(
				"QMultiMap<QString, QString>");

		cacheDir = tempDir.path();
		usermetricsDir = cacheDir.filePath("usermetrics");
		sourcesDir = usermetricsDir.filePath("sources");
		ASSERT_TRUE(usermetricsDir.mkpath("sources"));

		factory.reset(new NiceMock<MockFactory>);
		fileUtils.reset(new FileUtils);
		resultTransport.reset(new StrictMock<MockResultTransport>);
	}

	Service::Ptr newService() {
		return Service::Ptr(
				new ServiceImpl(cacheDir.path(), QDir(TEST_INFOGRAPHICS_DIR),
						fileUtils, resultTransport, *factory));
	}

	QDir applicationDir(const QString &sourceName) {
		QDir applicationDir = cacheDir.filePath(sourceName);
		return applicationDir.filePath("usermetrics");
	}

	void createSource(const QString &name) {
		QFile source(sourcesDir.filePath(name));
		ASSERT_TRUE(source.open(QIODevice::WriteOnly));
		ASSERT_TRUE(source.write("blah"));
		source.close();
	}

	QString metricPath(const QString &sourceName, const QString &metricName) {
		QDir applicationDir = cacheDir.filePath(sourceName);
		QDir applicationUsermetricsDir = applicationDir.filePath("usermetrics");
		return applicationUsermetricsDir.filePath(metricName);
	}

	void createMetric(const QString &sourceName, const QString &metricName) {
		QDir applicationDir = cacheDir.filePath(sourceName);
		QDir applicationUsermetricsDir = applicationDir.filePath("usermetrics");
		applicationDir.mkpath("usermetrics");

		QFile metric(applicationUsermetricsDir.filePath(metricName));
		ASSERT_TRUE(metric.open(QIODevice::WriteOnly));
		ASSERT_TRUE(metric.write("blah"));
		metric.close();
	}

	QSharedPointer<MockFactory> factory;

	FileUtils::Ptr fileUtils;

	QSharedPointer<MockResultTransport> resultTransport;

	QTemporaryDir tempDir;

	QDir cacheDir;

	QDir usermetricsDir;

	QDir sourcesDir;
};

TEST_F(TestServiceImpl, Startup) {
	QMultiMap<QString, QString> changedSources;
	changedSources.insert("com.ubuntu.camera",
			metricPath("com.ubuntu.camera", "camera-photos.json"));
	changedSources.insert("com.ubuntu.camera",
			metricPath("com.ubuntu.camera", "camera-videos.json"));

	QMultiMap<QString, QString> allSources;
	allSources.insert("com.ubuntu.camera",
			metricPath("com.ubuntu.camera", "camera-photos.json"));
	allSources.insert("com.ubuntu.camera",
			metricPath("com.ubuntu.camera", "camera-videos.json"));

	QSharedPointer<MockInfographic> iterateInfographic(
			new NiceMock<MockInfographic>());
	ON_CALL(*iterateInfographic, isValid()).WillByDefault(Return(true));
	EXPECT_CALL(*iterateInfographic,
			sourcesChanged(changedSources, allSources));
	EXPECT_CALL(*factory, newInfographic(HasPath(TEST_ITERATE_INFOGRAPHIC))).WillOnce(
			Return(iterateInfographic));

	QSharedPointer<MockInfographic> aggregateInfographic(
			new NiceMock<MockInfographic>());
	ON_CALL(*aggregateInfographic, isValid()).WillByDefault(Return(true));
	EXPECT_CALL(*aggregateInfographic,
			sourcesChanged(changedSources, allSources));
	EXPECT_CALL(*factory, newInfographic(HasPath(TEST_AGGREGATE_INFOGRAPHIC))).WillOnce(
			Return(aggregateInfographic));

	EXPECT_CALL(*resultTransport, clear());

	createSource("com.ubuntu.camera_camera_2.9.1.224.json");
	createMetric("com.ubuntu.camera", "camera-photos.json");
	createMetric("com.ubuntu.camera", "camera-videos.json");
	SourceDirectory::Ptr source(
			new SourceDirectoryImpl(applicationDir("com.ubuntu.camera"),
					fileUtils));
	EXPECT_CALL(*factory, newSourceDirectory(applicationDir("com.ubuntu.camera"))).WillOnce(
			Return(source));

	Service::Ptr service(newService());
}

} // namespace
