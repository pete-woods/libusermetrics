/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#include <libusermetricsoutput/InfographicListImpl.h>

#include <QDebug>
#include <QSignalSpy>
#include <QStringList>
#include <QTemporaryDir>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace UserMetricsOutput;
using namespace testing;

namespace {

class TestInfographicListImpl: public Test {
protected:
	void SetUp() {
		qRegisterMetaType<QVector<int>>("QVector<int>");
		usermetricsDir = tempDir.path();
	}

	QDir createUserDir(unsigned int uid) {
		QString name(QString::number(uid));
		usermetricsDir.mkpath(name);
		return QDir(usermetricsDir.filePath(name));
	}

	QString createInfographic(unsigned int uid, const QString &visualizer,
			const QString &name, const QByteArray &contents) {
		QDir userDir(createUserDir(uid));

		userDir.mkpath(visualizer);
		QDir visualizerDir(userDir.filePath(visualizer));

		QFile output(visualizerDir.filePath(name));
		output.open(QIODevice::WriteOnly);
		output.write(contents);
		output.close();

		return output.fileName();
	}

	QStringList contents(InfographicList &model, unsigned int count) {
		QStringList contents;
		for (unsigned int i(0); i < count; ++i) {
			contents << model.path();
			model.next();
		}
		contents.sort();
		return contents;
	}

	QTemporaryDir tempDir;

	QDir usermetricsDir;
};

TEST_F(TestInfographicListImpl, ModelUpdated) {
	InfographicListImpl list(tempDir.path());
	EXPECT_EQ(QStringList(), contents(list, 0));

	QSignalSpy dataChangedSpy(&list, SIGNAL(pathChanged(const QString &)));

	list.setUid(1234);

	createInfographic(1234, "com.ubuntu.camera", "apple-1234.svg", "<apple/>");
	QString appleFile = createInfographic(1234, "com.ubuntu.camera",
			"apple-2345.svg", "<apple/>");

	createInfographic(1234, "com.ubuntu.camera", "banana-1234.svg",
			"<oldBanana/>");
	createInfographic(1234, "com.ubuntu.camera", "banana-2345.svg",
			"<oldBanana/>");
	QString bananaFile = createInfographic(1234, "com.ubuntu.camera",
			"banana-3456.svg", "<banana/>");

	createInfographic(1234, "com.ubuntu.camera", "cherry-1234.svg",
			"<oldCherry/>");
	createInfographic(1234, "com.ubuntu.camera", "cherry-2345.svg",
			"<oldCherry/>");
	QString cherryFile = createInfographic(1234, "com.ubuntu.camera",
			"cherry-3456.svg", "<cherry/>");

	dataChangedSpy.wait();
	ASSERT_FALSE(dataChangedSpy.isEmpty());
	dataChangedSpy.clear();

	EXPECT_EQ(QStringList() << appleFile << bananaFile << cherryFile,
			contents(list, 3));

	QString fooFile = createInfographic(1234, "com.ubuntu.foo", "foo-1234.svg",
			"<foo/>");

	dataChangedSpy.wait();
	ASSERT_FALSE(dataChangedSpy.isEmpty());

	EXPECT_EQ(QStringList() << appleFile << bananaFile << cherryFile << fooFile,
			contents(list, 4));
}

} // namespace
