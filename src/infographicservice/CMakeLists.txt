
set(INFOGRAPHICSERVICE_SOURCES
	Service.cpp
)

qt5_add_dbus_adaptor(
	INFOGRAPHICSERVICE_SOURCES
	${INFOGRAPHIC_SERVICE_XML}
	infographicservice/Service.h
	InfographicService::Service
	InfographicServiceAdaptor
)

add_library(
	infographicservice
	STATIC
	${INFOGRAPHICSERVICE_SOURCES}
)

qt5_use_modules(
	infographicservice
	Core
	DBus
)

############################
# Service executable
############################

add_executable(
	infographicservice-bin
	main.cpp
)

set_target_properties(
	infographicservice-bin
	PROPERTIES
	OUTPUT_NAME "infographicservice"
)

target_link_libraries(
	infographicservice-bin
	infographicservice
)

qt5_use_modules(
	infographicservice-bin
	Core
)

#########################
# Installation
#########################

install(
  TARGETS infographicservice-bin
  RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)
