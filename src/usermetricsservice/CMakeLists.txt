
#set(USERMETRICSSERVICE_SOURCES
#)

#add_library(
#	usermetricsservice
#	STATIC
#	${USERMETRICSSERVICE_SOURCES}
#)

#target_link_libraries(
#	usermetricsservice
#	${APPARMOR_LIBRARIES}
#)

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

#target_link_libraries(
#	usermetricsservice-bin
#	usermetricsservice
#)

#########################
# Installation
#########################

install(
  TARGETS usermetricsservice-bin
  RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)
