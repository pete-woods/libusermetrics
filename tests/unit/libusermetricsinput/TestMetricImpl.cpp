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

#include <libusermetricsinput/MetricImpl.h>
#include <unit/libusermetricsinput/Mocks.h>
#include <testutils/QStringPrinter.h>
#include <testutils/QVariantPrinter.h>
#include <testutils/QVariantListPrinter.h>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QTemporaryDir>
#include <QtCore/QVariantList>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace testing;
using namespace UserMetricsInput;
using namespace UserMetricsInput::Mocks;

namespace {

class TestMetricCommon {
protected:
	explicit TestMetricCommon() {
		assert(temporaryDir.isValid());

		factory.reset(new NiceMock<MockFactory>());
		ON_CALL(*factory, currentDate()).WillByDefault(
				Return(QDate(2001, 03, 1)));

		metricDir.setPath(temporaryDir.path());
	}

	virtual ~TestMetricCommon() {
	}

	QSharedPointer<MetricImpl> newMetric(const MetricParameters &parameters) {
		QSharedPointer<MetricImpl> metric(
				new MetricImpl(metricDir, parameters, factory));
		metric->setSelf(metric);
		return metric;
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

	QVariantMap readData(const QString &id) {
		QVariantMap map;
		QFile file(metricDir.filePath(id + ".json"));
		EXPECT_TRUE(file.open(QIODevice::ReadOnly));
		QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
		file.close();
		map = doc.toVariant().toMap();
		return map;
	}

	void EXPECT_DATA_EQ(const QString &id, const QVariantList &data) {
		QVariantMap map(readData(id));
		EXPECT_EQ(data, map["data"].toList());
	}

	void EXPECT_SCALED_DATA_EQ(const QString &id, const QVariantList &data) {
		QVariantMap map(readData(id));
		EXPECT_EQ(data, map["scaledData"].toList());
	}

	void EXPECT_LAST_UPDATED_EQ(const QString &id, const QDate &lastUpdated) {
		QVariantMap map(readData(id));
		EXPECT_EQ(lastUpdated, map["lastUpdated"].toDate());
	}

	void EXPECT_OPTIONS_EQ(const QString &id, const QVariantMap &options) {
		QVariantMap map(readData(id));
		EXPECT_EQ(options, map["options"].toMap());
	}

	QSharedPointer<MockFactory> factory;

	QTemporaryDir temporaryDir;

	QDir metricDir;
};

class TestMetricUpdates: public Test, public TestMetricCommon {
};

TEST_F(TestMetricUpdates, UpdateData) {
	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 5)));

	QSharedPointer<MetricImpl> twitter(newMetric(MetricParameters("twitter")));

	QVariantList first( { 5.0, 4.0, 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0 });
	QVariantList expected( { 10.0, 9.0, 8.0, 7.0, 6.0, 3.0, 2.0, 1.0 });

	// first update happens on the 5th of the month
	twitter->update(first);
	EXPECT_DATA_EQ("twitter", first);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 5));

	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 8)));

	// second update happens on the 8th of the month
	// -> 3 new data points and 2 overwritten
	twitter->update(second);
	EXPECT_DATA_EQ("twitter", expected);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 8));
}

TEST_F(TestMetricUpdates, UpdateDataWithGap) {
	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 5)));

	QSharedPointer<MetricImpl> twitter(newMetric(MetricParameters("twitter")));

	QVariantList first( { 5.0, 4.0, 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0 });
	QVariantList expected;
	expected.append(second);
	for (int i(0); i < 5; ++i) {
		expected.append(QVariant());
	}
	expected.append(first);

	// first update happens on the 5th of the month
	twitter->update(first);
	EXPECT_DATA_EQ("twitter", first);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 5));

	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 15)));

	// second update happens on the 15th of the month
	// -> 5 new data points, 5 nulls, and none overwritten
	twitter->update(second);
	EXPECT_DATA_EQ("twitter", expected);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 15));
}

TEST_F(TestMetricUpdates, UpdateDataTotallyOverwrite) {
	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 5)));

	MetricImpl twitter(metricDir,
			MetricParameters("twitter").formatString("foo %1").emptyDataString(
					"empty"), factory);

	QVariantList first( { 3.0, 2.0, 1.0 });
	QVariantList second( { 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0 });
	QVariantList &expected(second);

	// first update happens on the 5th of the month
	twitter.update(first);
	EXPECT_DATA_EQ("twitter", first);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 5));

	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 01, 7)));

	// second update happens on the 7th of the month
	// -> 2 new data points, 3 overwrites, and 2 new appends
	twitter.update(second);
	EXPECT_DATA_EQ("twitter", expected);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 01, 7));
}

TEST_F(TestMetricUpdates, IncrementOverSeveralDays) {
	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 03, 1)));

	QSharedPointer<MetricImpl> twitter(newMetric(MetricParameters("twitter")));

	QVariantList expected1 { 1.0 };
	QVariantList expected2 { 1.0, 1.0 };
	QVariantList expected3 { 1.0, 1.0, 1.0 };

	twitter->increment(1.0);
	EXPECT_DATA_EQ("twitter", expected1);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 03, 1));

	EXPECT_CALL(*factory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 03, 2)));

	twitter->increment(1.0);
	EXPECT_DATA_EQ("twitter", expected2);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 03, 2));

	EXPECT_CALL(*factory, currentDate()).WillRepeatedly(
			Return(QDate(2001, 03, 3)));

	twitter->increment(1.0);
	EXPECT_DATA_EQ("twitter", expected3);
	EXPECT_LAST_UPDATED_EQ("twitter", QDate(2001, 03, 3));
}

TEST_F(TestMetricUpdates, StoreMaximumOf62Days) {
	ON_CALL(*factory, currentDate()).WillByDefault(Return(QDate(2001, 03, 5)));

	QSharedPointer<MetricImpl> twitter(newMetric(MetricParameters("twitter")));

	// create some data that's way too big
	QVariantList input;
	for (int i(0); i < 100; ++i) {
		input << 1.0;
	}
	twitter->update(input);

	// create some data that's way too big
	QVariantList expected;
	for (int i(0); i < 62; ++i) {
		expected << 1.0;
	}

	EXPECT_DATA_EQ("twitter", expected);
}

TEST_F(TestMetricUpdates, TestCanAddDataAndUpdate) {
	MetricPtr metric(newMetric(MetricParameters("data-source-id")));

	{
		MetricUpdatePtr update(metric->update());
		update->addData(100.0);
		update->addNull();
		update->addData(-50.0);
		update->addData(-22.0);
	}

	EXPECT_DATA_EQ("data-source-id",
			QVariantList() << 100.0 << QVariant() << -50.0 << -22.0);
	EXPECT_LAST_UPDATED_EQ("data-source-id", QDate(2001, 03, 1));
}

TEST_F(TestMetricUpdates, TestAddMultipleDataSourcesAndUsers) {
	MetricPtr metricOne(newMetric(MetricParameters("data-source-one")));
	MetricPtr metricTwo(newMetric(MetricParameters("data-source-two")));

	{
		MetricUpdatePtr updateOne(metricOne->update());
		updateOne->addData(100.0);

		MetricUpdatePtr updateTwo(metricTwo->update());
		updateTwo->addData(50.0);
	}

	EXPECT_DATA_EQ("data-source-one", QVariantList() << 100.0);
	EXPECT_DATA_EQ("data-source-two", QVariantList() << 50.0);
}

TEST_F(TestMetricUpdates, TestCanAddDataAndUpdateToday) {
	MetricPtr metric(newMetric(MetricParameters("data-source-id")));

	metric->update(2.5);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 2.5);

	metric->update(1.3);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 1.3);

	EXPECT_LAST_UPDATED_EQ("data-source-id", QDate(2001, 03, 1));
}

TEST_F(TestMetricUpdates, TestCanAddDataAndIncrement) {
	MetricPtr metric(newMetric(MetricParameters("data-source-id")));

	metric->increment(1.0);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 1.0);

	metric->increment(3.0);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 4.0);

	EXPECT_LAST_UPDATED_EQ("data-source-id", QDate(2001, 03, 1));
}

TEST_F(TestMetricUpdates, TestCanAddNullAndIncrement) {
	MetricPtr metric(newMetric(MetricParameters("data-source-id")));

	{
		MetricUpdatePtr update(metric->update());
		update->addNull();
	}
	EXPECT_DATA_EQ("data-source-id", QVariantList() << QVariant());

	metric->increment(5.2);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 5.2);

	EXPECT_LAST_UPDATED_EQ("data-source-id", QDate(2001, 03, 1));
}

TEST_F(TestMetricUpdates, TestMinimum) {
	QSharedPointer<MetricImpl> metric(
			newMetric(MetricParameters("data-source-id").minimum(-5.0)));

	metric->update(QVariantList() << 5.0 << 0.0);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 5.0 << 0.0);
	EXPECT_SCALED_DATA_EQ("data-source-id", QVariantList() << 1.0 << 0.5);

	QVariantMap expected;
	expected["minimum"] = -5.0;
	EXPECT_OPTIONS_EQ("data-source-id", expected);
}

TEST_F(TestMetricUpdates, TestMaximum) {
	QSharedPointer<MetricImpl> metric(
			newMetric(MetricParameters("data-source-id").maximum(4.0)));

	metric->update(QVariantList() << -4.0 << 0.0);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << -4.0 << 0.0);
	EXPECT_SCALED_DATA_EQ("data-source-id", QVariantList() << 0.0 << 0.5);

	QVariantMap expected;
	expected["maximum"] = 4.0;
	EXPECT_OPTIONS_EQ("data-source-id", expected);
}

TEST_F(TestMetricUpdates, TestMiniumMaximum) {
	QSharedPointer<MetricImpl> metric(
			newMetric(
					MetricParameters("data-source-id").minimum(1.0).maximum(
							5.0)));

	metric->update(QVariantList() << 2.0);
	EXPECT_DATA_EQ("data-source-id", QVariantList() << 2.0);
	EXPECT_SCALED_DATA_EQ("data-source-id", QVariantList() << 0.25);

	QVariantMap expected;
	expected["minimum"] = 1.0;
	expected["maximum"] = 5.0;
	EXPECT_OPTIONS_EQ("data-source-id", expected);
}

class TestDataSetParamData {
public:
	TestDataSetParamData(const QVariantList &input,
			const QVariantList &expectedData) :
			m_input(input), m_expectedData(expectedData) {
	}

	virtual ~TestDataSetParamData() {
	}

	QVariantList m_input;
	QVariantList m_expectedData;
};

static void PrintTo(const TestDataSetParamData& testData, ostream* os) {
	*os << "TestData(";
	PrintTo(testData.m_input, os);
	*os << ", ";
	PrintTo(testData.m_expectedData, os);
	*os << ")";
}

class TestDataScaling: public TestWithParam<TestDataSetParamData>,
		public TestMetricCommon {
};

TEST_P(TestDataScaling, DataSetting) {
	TestDataSetParamData testData(GetParam());
	QSharedPointer<MetricImpl> metric(newMetric(MetricParameters("metric")));
	metric->update(testData.m_input);
	EXPECT_SCALED_DATA_EQ("metric", testData.m_expectedData);
}

INSTANTIATE_TEST_CASE_P(ScalesData, TestDataScaling,
		Values(
				TestDataSetParamData(
						QVariantList( { -10.0, -5.0, 0.0, 5.0, 10.0 }),
						QVariantList( { 0.0, 0.25, 0.5, 0.75, 1.0 }))));

INSTANTIATE_TEST_CASE_P(TurnsBlankStringIntoNull, TestDataScaling,
		Values(
				TestDataSetParamData(QVariantList( { QVariant() }),
						QVariantList( { QVariant() }))));

INSTANTIATE_TEST_CASE_P(TurnsBlankStringIntoNullWithOtherData, TestDataScaling,
		Values(
				TestDataSetParamData(QVariantList( { 100.0, QVariant(), 50.0,
						0.0 }), QVariantList( { 1.0, QVariant(), 0.5, 0.0 }))));

INSTANTIATE_TEST_CASE_P(TurnsSingleValueInto0Point5, TestDataScaling,
		Values(TestDataSetParamData(QVariantList( { 127.0 }), QVariantList( {
				0.5 }))));

INSTANTIATE_TEST_CASE_P(TurnsZeroRangeInto0Point5s, TestDataScaling,
		Values(
				TestDataSetParamData(QVariantList( { 27.0, 27.0 }),
						QVariantList( { 0.5, 0.5 }))));

INSTANTIATE_TEST_CASE_P(TurnsZeroRangeInto0Point5sMoreValues, TestDataScaling,
		Values(
				TestDataSetParamData(QVariantList( { 150.0, 150.0, 150.0 }),
						QVariantList( { 0.5, 0.5, 0.5 }))));

}	// namespace
