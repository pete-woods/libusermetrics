
set(USERMETRICSSERVICE_SOURCES
	Executor.cpp
	Factory.cpp
	FileUtils.cpp
	Infographic.cpp
	InfographicImpl.cpp
	QProcessExecutor.cpp
	ResultTransport.cpp
	ResultTransportImpl.cpp
	SourceDirectory.cpp
	SourceDirectoryImpl.cpp
	Service.cpp
	ServiceImpl.cpp
)

add_library(
	usermetricsservice
	STATIC
	${USERMETRICSSERVICE_SOURCES}
)

target_link_libraries(
	usermetricsservice
	usermetricscommon
)

qt5_use_modules(
	usermetricsservice
	Core
	DBus
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
# Sandbox executable
#########################

add_executable(
	aa-exec
	aa-exec.cpp
)

add_definitions(
	-DAA_EXEC_PATH="${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics/aa-exec"
)

#########################
# Installation
#########################

install(
	TARGETS
		usermetricsservice-bin
		aa-exec
	RUNTIME DESTINATION
		"${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)
