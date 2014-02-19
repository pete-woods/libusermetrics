/*
 * Service.h
 *
 *  Created on: 19 Feb 2014
 *      Author: pete
 */

#ifndef USERMETRICSSERVICE_SERVICE_H_
#define USERMETRICSSERVICE_SERVICE_H_

#include <QObject>
#include <QSharedPointer>

namespace UserMetricsService {

class Service: public QObject {
public:
	typedef QSharedPointer<Service> Ptr;

	Service();

	virtual ~Service();
};

}

#endif /* USERMETRICSSERVICE_SERVICE_H_ */
