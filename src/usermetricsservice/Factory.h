/*
 * Factory.h
 *
 *  Created on: 19 Feb 2014
 *      Author: pete
 */

#ifndef FACTORY_H_
#define FACTORY_H_

#include <usermetricsservice/Service.h>

namespace UserMetricsService {

class Factory {
public:
	Factory();

	virtual ~Factory();

	virtual Service::Ptr singletonService();

protected:
	Service::Ptr m_service;
};

} /* namespace UserMetricsService */

#endif /* FACTORY_H_ */
