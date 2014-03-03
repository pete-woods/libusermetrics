
set(USERMETRICS_COMMON_SOURCES
	DateFactory.cpp
	DateFactoryImpl.cpp
	Localisation.cpp
)

qt5_add_dbus_interface(
	USERMETRICS_COMMON_SOURCES
	${INFOGRAPHIC_SERVICE_XML}
	InfographicsInterface
)

add_library(
	usermetricscommon
	STATIC
	${USERMETRICS_COMMON_SOURCES}
)

qt5_use_modules(
	usermetricscommon
	Core
	DBus
)
