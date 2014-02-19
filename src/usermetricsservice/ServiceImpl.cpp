/*
 * ServiceImpl.cpp
 *
 *  Created on: 19 Feb 2014
 *      Author: pete
 */

#include <usermetricsservice/ServiceImpl.h>

#include <QDebug>
#include <QStringList>
#include <stdexcept>

using namespace UserMetricsService;
using namespace std;

ServiceImpl::ServiceImpl(const QDir &cacheDirectory) {
	QDir usermetricsDirectory(cacheDirectory.filePath("usermetrics"));

	if (!usermetricsDirectory.mkpath("infographics")) {
		throw logic_error("Cannot write to cache directory");
	}

	m_infographicDirectory = usermetricsDirectory.filePath("infographics");
	m_infographicWatcher.addPath(m_infographicDirectory.path());
}

ServiceImpl::~ServiceImpl() {
}

void ServiceImpl::infographicsChanged() {
	QStringList names;
	foreach(const QFileInfo &fileInfo , m_infographicDirectory.entryInfoList(
					QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)){
		names << fileInfo.fileName();
	}
	qDebug() << names;
}
