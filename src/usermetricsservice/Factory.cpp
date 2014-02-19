/*
 * Factory.cpp
 *
 *  Created on: 19 Feb 2014
 *      Author: pete
 */

#include <usermetricsservice/Factory.h>
#include <usermetricsservice/ServiceImpl.h>

using namespace UserMetricsService;

Factory::Factory() {
}

Factory::~Factory() {
}

Service::Ptr Factory::singletonService() {
	if (!m_service) {
		m_service.reset(new ServiceImpl(QDir::home().filePath(".cache")));
	}
	return m_service;
}
