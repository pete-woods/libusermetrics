
set(USERMETRICS_COMMON_SOURCES
	DateFactory.cpp
	DateFactoryImpl.cpp
	Localisation.cpp
)

add_library(
	usermetricscommon
	STATIC
	${USERMETRICS_COMMON_SOURCES}
)

qt5_use_modules(
	usermetricscommon
	Core
)

