
set(USERMETRICSSERVICE_SOURCES
	Factory.cpp
	Infographic.cpp
	InfographicImpl.cpp
	Service.cpp
	ServiceImpl.cpp
)

add_library(
	usermetricsservice
	STATIC
	${USERMETRICSSERVICE_SOURCES}
)

#target_link_libraries(
#	usermetricsservice
#	${APPARMOR_LIBRARIES}
#)

qt5_use_modules(
	usermetricsservice
	Core
)

############################
# Service executable
############################

add_executable(
	usermetricsservice-bin
	main.cpp
)

set_target_properties(
	usermetricsservice-bin
	PROPERTIES
	OUTPUT_NAME "usermetricsservice"
)

target_link_libraries(
	usermetricsservice-bin
	usermetricsservice
)

qt5_use_modules(
	usermetricsservice-bin
	Core
)

#########################
# Installation
#########################

install(
  TARGETS usermetricsservice-bin
  RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)
