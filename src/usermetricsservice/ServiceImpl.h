/*
 * ServiceImpl.h
 *
 *  Created on: 19 Feb 2014
 *      Author: pete
 */

#ifndef SERVICEIMPL_H_
#define SERVICEIMPL_H_

#include <usermetricsservice/Service.h>

#include <QFileSystemWatcher>
#include <QDir>

namespace UserMetricsService {

class ServiceImpl: public Service {
Q_OBJECT

public:
	ServiceImpl(const QDir &cacheDirectory);

	virtual ~ServiceImpl();

protected Q_SLOTS:
	void infographicsChanged();

protected:
	QDir m_infographicDirectory;

	QFileSystemWatcher m_infographicWatcher;
};

} /* namespace UserMetricsInfographic */

#endif /* SERVICEIMPL_H_ */
