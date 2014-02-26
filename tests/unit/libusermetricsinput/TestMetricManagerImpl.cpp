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

#include <QtCore/QDebug>
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

		cacheDir = temporaryDir.path();

		QDir usermetricsDir(cacheDir.filePath("usermetrics"));
		sourcesDir = usermetricsDir.filePath("sources");

		applicationIdOne = "app-id-1";
		metricDirOne = sourcesDir.filePath(applicationIdOne);

		applicationIdTwo = "app-id-2";
		applicationIdTwoFull = "app-id-2_app_1.2.12345";
		metricDirTwo = sourcesDir.filePath(applicationIdTwo);
	}

	static void EXPECT_DIR_CONTAINS(const QDir &dir,
			const QStringList &expectedNames) {
		qDebug() << "checking dir" << dir << "for" << expectedNames;
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

	QDir cacheDir;

	QDir sourcesDir;

	QString applicationIdOne;

	QDir metricDirOne;

	QString applicationIdTwo;

	QString applicationIdTwoFull;

	QDir metricDirTwo;
};

TEST_F(TestMetricManagerImpl, TestWillGetSameMetricForSameId) {
	MetricParameters params("data-source-id");
	params.formatString("format string %1");

	MetricManagerPtr manager(
			new MetricManagerImpl(factory, cacheDir, applicationIdOne));
	EXPECT_CALL(*factory, newMetric(metricDirOne,
					params, _)).WillOnce(Return(metric));
	MetricPtr newMetric(manager->add(params));
	EXPECT_EQ(metric, newMetric);

	newMetric = manager->add(params);
	EXPECT_EQ(metric, newMetric);

	EXPECT_DIR_CONTAINS(sourcesDir, QStringList() << applicationIdOne);
}

TEST_F(TestMetricManagerImpl, CreateTwoDifferentApplications) {
	MetricParameters params("data-source-id");
	params.formatString("format string %1");

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, cacheDir, applicationIdOne));
		EXPECT_CALL(*factory, newMetric(metricDirOne,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add(params));
		EXPECT_EQ(metric, newMetric);
	}

	{
		MetricManagerPtr manager(
				new MetricManagerImpl(factory, cacheDir, applicationIdTwoFull));
		EXPECT_CALL(*factory, newMetric(metricDirTwo,
						params, _)).WillOnce(Return(metric));
		MetricPtr newMetric(manager->add("data-source-id", "format string %1"));
		EXPECT_EQ(metric, newMetric);
	}

	EXPECT_DIR_CONTAINS(sourcesDir,
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

	EXPECT_DIR_CONTAINS(sourcesDir, QStringList() << applicationIdOne);
}

}// namespace
