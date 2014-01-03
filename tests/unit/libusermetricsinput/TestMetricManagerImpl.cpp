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

#include <libusermetricsinput/MetricManagerImpl.h>

#include <testutils/QStringPrinter.h>
#include <unit/libusermetricsinput/Mocks.h>

#include <QtCore/QTemporaryDir>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace testing;
using namespace UserMetricsInput;
using namespace UserMetricsInput::Mocks;

namespace {

class TestMetricManagerImpl: public Test {
protected:
	void SetUp() {
		ASSERT_TRUE(temporaryDir.isValid());

		factory.reset(new NiceMock<MockFactory>());

		metric.reset(new NiceMock<MockMetric>());

		metricsDir.setPath(QDir(temporaryDir.path()).filePath("usermetrics"));

		applicationIdOne = "app-id-1";
		metricDirOne.setPath(metricsDir.filePath(applicationIdOne));

		applicationIdTwo = "app-id-2";
		metricDirTwo.setPath(metricsDir.filePath(applicationIdTwo));
	}

	static void EXPECT_DIR_CONTAINS(const QDir &dir,
			const QStringList &expectedNames) {
		QStringList names;
		for (const QFileInfo &fileInfo : dir.entryInfoList(
				QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
			names << fileInfo.fileName();
		}
		EXPECT_EQ(expectedNames, names);
	}

	QSharedPointer<MockFactory> factory;

	QSharedPointer<MockMetric> metric;

	QTemporaryDir temporaryDir;

	QDir metricsDir;

	QString applicationIdOne;

	QDir metricDirOne;

	QString applicationIdTwo;

	QDir metricDirTwo;
};

TEST_F(TestMetricManagerImpl, TestWillGetSameMetricForSameId) {
	MetricParameters params("data-source-id");
	params.formatString("format string %1");

	MetricManagerPtr manager(
			new MetricManagerImpl(factory, temporaryDir.path(),
					applicationIdOne));
	EXPECT_CALL(*factory, newMetric(metricDirOne,
					params, _)).WillOnce(Return(metric));
	MetricPtr newMetric(manager->add(params));
	EXPECT_EQ(metric, newMetric);

	newMetric = manager->add(params);
	EXPECT_EQ(metric, newMetric);

	EXPECT_DIR_CONTAINS(QDir(temporaryDir.path()),
			QStringList() << "usermetrics");
	EXPECT_DIR_CONTAINS(metricsDir, QStringList() << applicationIdOne);
}

TEST_F(TestMetricManagerImpl, CreateTwoDifferentApplications) {
	MetricParameters params("data-source-id");
	params.formatString("format string %1");

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, temporaryDir.path(),
						applicationIdOne));
		EXPECT_CALL(*factory, newMetric(metricDirOne,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add(params));
		EXPECT_EQ(metric, newMetric);
	}

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, temporaryDir.path(),
						applicationIdTwo));
		EXPECT_CALL(*factory, newMetric(metricDirTwo,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add("data-source-id", "format string %1"));
		EXPECT_EQ(metric, newMetric);
	}

	EXPECT_DIR_CONTAINS(QDir(temporaryDir.path()),
			QStringList() << "usermetrics");
	EXPECT_DIR_CONTAINS(metricsDir,
			QStringList() << applicationIdOne << applicationIdTwo);
}

TEST_F(TestMetricManagerImpl, TestCanAddDataSourceMultipleTimes) {
	MetricParameters params("data-source-id");
	params.formatString("format string %1");

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, temporaryDir.path(),
						applicationIdOne));
		EXPECT_CALL(*factory, newMetric(metricDirOne,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add(params));
		EXPECT_EQ(metric, newMetric);
	}

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, temporaryDir.path(),
						applicationIdOne));
		EXPECT_CALL(*factory, newMetric(metricDirOne,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add("data-source-id", "format string %1"));
		EXPECT_EQ(metric, newMetric);
	}

	EXPECT_DIR_CONTAINS(QDir(temporaryDir.path()),
			QStringList() << "usermetrics");
	EXPECT_DIR_CONTAINS(metricsDir, QStringList() << applicationIdOne);
}

} // namespace
