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

#include <libusermetricsinput/usermetricsinput.h>

#include <testutils/QVariantListPrinter.h>

#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QTemporaryDir>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace {

class TestUserMetricInputCAPI: public Test {
protected:
	TestUserMetricInputCAPI() {
		m_originalHomeDir = qgetenv("HOME");
		m_originalAppId = qgetenv("APP_ID");
		EXPECT_TRUE(m_temporaryDir.isValid());
		qputenv("HOME", qPrintable(m_temporaryDir.path()));
		qputenv("APP_ID", "test-app");
	}

	~TestUserMetricInputCAPI() {
		qputenv("HOME", m_originalHomeDir);
		qputenv("APP_ID", m_originalAppId);
	}

	QVariantMap readData(const QString &id) {
		QDir cacheDir(QDir(m_temporaryDir.path()).filePath(".cache"));
		QDir applicationDir(cacheDir.filePath("test-app"));
		QDir usermetricsDir(applicationDir.filePath("usermetrics"));

		QFile file(usermetricsDir.filePath(id + ".libusermetrics.json"));
		EXPECT_TRUE(file.open(QIODevice::ReadOnly));
		QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
		file.close();
		return doc.toVariant().toMap();
	}

	QTemporaryDir m_temporaryDir;

	QByteArray m_originalHomeDir;

	QByteArray m_originalAppId;
};

TEST_F(TestUserMetricInputCAPI, TestBasicFunctionality) {
	UserMetricsInputMetricManager metricManager =
			usermetricsinput_metricmanager_new();

	UserMetricsInputMetricParameters p1 = usermetricsinput_metricparameters_new(
			"data-source-id-capi");
	usermetricsinput_metricparameters_set_format_string(p1,
			"format string c-api %1");
	usermetricsinput_metricparameters_set_empty_data_string(p1,
			"empty data string");
	usermetricsinput_metricparameters_set_text_domain(p1, "text domain");

	UserMetricsInputMetric metric = usermetricsinput_metricmanager_add(
			metricManager, p1);
	usermetricsinput_metricparameters_delete(p1);

	UserMetricsInputMetricParameters p2 = usermetricsinput_metricparameters_new(
			"data-source-id-capi-min");
	usermetricsinput_metricparameters_set_format_string(p2,
			"format string c-api %1 min");
	usermetricsinput_metricparameters_set_empty_data_string(p2,
			"empty data string min");
	usermetricsinput_metricparameters_set_text_domain(p2, "text domain min");
	usermetricsinput_metricparameters_set_minimum(p2, -4.0);
	usermetricsinput_metricparameters_set_maximum(p2, 5.0);
	usermetricsinput_metricparameters_set_type(p2, METRIC_TYPE_SYSTEM);

	UserMetricsInputMetric metric2 = usermetricsinput_metricmanager_add(
			metricManager, p2);
	usermetricsinput_metricparameters_delete(p2);

	UserMetricsInputMetricUpdate metricUpdate = usermetricsinput_metric_update(
			metric, "username_capi");
	usermetricsinput_metricupdate_add_data(metricUpdate, 1.0);
	usermetricsinput_metricupdate_add_null(metricUpdate);
	usermetricsinput_metricupdate_add_data(metricUpdate, 0.1);
	usermetricsinput_metricupdate_delete(metricUpdate);

	usermetricsinput_metric_update_today(metric2, 0.0, "");

	{
		QVariantMap data(readData("data-source-id-capi"));
		EXPECT_EQ(QString("data-source-id-capi"), data["id"].toString());
		EXPECT_EQ(QString("format string c-api %1"),
				data["formatString"].toString());
		EXPECT_EQ(QString("empty data string"),
				data["emptyDataString"].toString());
		EXPECT_EQ(QString("text domain"), data["textDomain"].toString());
	}

	{
		QVariantMap data(readData("data-source-id-capi-min"));
		EXPECT_EQ(QString("data-source-id-capi-min"), data["id"].toString());
		QVariantMap options;
		options["minimum"] = -4.0;
		options["maximum"] = 5.0;
		EXPECT_EQ(options, data["options"].toMap());
	}

	{
		QVariantMap source(readData("data-source-id-capi"));
		QVariantList data(source["data"].toList());
		ASSERT_EQ(3, data.size());
		EXPECT_FLOAT_EQ(1.0, data.at(0).toDouble());
		EXPECT_EQ(QVariant(), data.at(1));
		EXPECT_FLOAT_EQ(0.1, data.at(2).toDouble());
	}

	usermetricsinput_metric_increment(metric, 4.5, "");

	{
		QVariantMap source(readData("data-source-id-capi"));
		QVariantList data(source["data"].toList());
		ASSERT_EQ(3, data.size());
		EXPECT_FLOAT_EQ(5.5, data.at(0).toDouble());
		EXPECT_EQ(QVariant(), data.at(1));
		EXPECT_FLOAT_EQ(0.1, data.at(2).toDouble());
	}

	usermetricsinput_metric_update_today(metric, -3.5, "");

	{
		QVariantMap source(readData("data-source-id-capi"));
		QVariantList data(source["data"].toList());
		ASSERT_EQ(3, data.size());
		EXPECT_FLOAT_EQ(-3.5, data.at(0).toDouble());
		EXPECT_EQ(QVariant(), data.at(1));
		EXPECT_FLOAT_EQ(0.1, data.at(2).toDouble());
	}

	usermetricsinput_metricmanager_delete(metricManager);
}

} // namespace
