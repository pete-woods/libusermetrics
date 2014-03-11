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

	QStringList contents(QAbstractItemModel *model) {
		QStringList contents;
		for (int i(0); i < model->rowCount(); ++i) {
			contents << model->data(model->index(i, 0)).toString();
		}
		contents.sort();
		return contents;
	}

	QTemporaryDir tempDir;

	QDir usermetricsDir;
};

TEST_F(TestInfographicListImpl, ModelUpdated) {
	InfographicListImpl list(tempDir.path());
	QAbstractItemModel *model(list.infographics());

	QSignalSpy resetSpy(model, SIGNAL(modelReset()));

	list.setUid(1234);

	QString appleFile = createInfographic(1234, "com.ubuntu.camera",
			"apple.svg", "<apple/>");
	QString bananaFile = createInfographic(1234, "com.ubuntu.camera",
			"banana.svg", "<banana/>");

	resetSpy.wait();
	ASSERT_EQ(1, resetSpy.size());
	resetSpy.clear();

	EXPECT_EQ(QStringList() << appleFile << bananaFile, contents(model));

	QString fooFile = createInfographic(1234, "com.ubuntu.foo", "foo.svg",
			"<foo/>");

	resetSpy.wait();
	ASSERT_EQ(1, resetSpy.size());

	EXPECT_EQ(QStringList() << appleFile << bananaFile << fooFile,
			contents(model));
}

TEST_F(TestInfographicListImpl, FilesUpdatedSignal) {
	InfographicListImpl list(tempDir.path());
	QAbstractItemModel *model(list.infographics());

	QSignalSpy resetSpy(model, SIGNAL(modelReset()));
	QSignalSpy filesUpdatedSpy(&list, SIGNAL(filesUpdated(QStringList)));

	list.setUid(1234);

	createInfographic(1234, "com.ubuntu.camera", "apple.svg", "<apple/>");
	createInfographic(1234, "com.ubuntu.camera", "banana.svg", "<banana/>");

	resetSpy.wait();
	ASSERT_EQ(1, resetSpy.size());

	createInfographic(1234, "com.ubuntu.foo", "foo.svg", "<foo/>");
	QString bananaFile = createInfographic(1234, "com.ubuntu.camera",
			"banana.svg", "<bananaBanana/>");

	filesUpdatedSpy.wait();

	ASSERT_EQ(1, filesUpdatedSpy.size());
	ASSERT_EQ(QVariantList() << QVariant(QStringList() << bananaFile),
			filesUpdatedSpy.at(0));
}

} // namespace
