include("UseGSettings")

###########################
# Settings
###########################

add_schema("com.canonical.UserMetrics.gschema.xml")

install(
	DIRECTORY
		libusermetrics
	DESTINATION
		${CMAKE_INSTALL_DATADIR}
)

###########################
# Default infographic
###########################

configure_file(
	"default.json.in"
	"${CMAKE_CURRENT_BINARY_DIR}/default.json" @ONLY
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/default.json"
	DESTINATION
		"${CMAKE_INSTALL_DATADIR}/libusermetrics/infographics/"
)	

###########################
# Policy
###########################

set(
	DBUSCONFDIR
	/etc/dbus-1/system.d
)

install(
	FILES
	"com.canonical.Infographics.conf"
	DESTINATION ${DBUSCONFDIR}
)

###########################
# Dbus Interfaces
###########################

set(
	DBUSIFACEDIR
	"${CMAKE_INSTALL_DATADIR}/dbus-1/interfaces/"
)

install(
	FILES
	com.canonical.Infographics.xml
	DESTINATION ${DBUSIFACEDIR}
)

###########################
# Dbus Services
###########################

set(
	DBUSSERVICEDIR
	"${CMAKE_INSTALL_DATADIR}/dbus-1/system-services/"
)

set(INFOGRAPHICS_SERVICE
	"${CMAKE_CURRENT_BINARY_DIR}/com.canonical.Infographics.service"
)

configure_file(
	"com.canonical.Infographics.service.in"
	${INFOGRAPHICS_SERVICE}
	@ONLY
)

install(
	FILES
	${INFOGRAPHICS_SERVICE}
	DESTINATION ${DBUSSERVICEDIR}
)

##############################
# Usermetrics session service
##############################

configure_file(
	"usermetricsservice.conf.in"
	"${CMAKE_CURRENT_BINARY_DIR}/usermetricsservice.conf" @ONLY
)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/usermetricsservice.conf"
	DESTINATION "${CMAKE_INSTALL_DATADIR}/upstart/sessions"
)
