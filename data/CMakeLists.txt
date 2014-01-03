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
# Policy
###########################

set(
	DBUSCONFDIR
	/etc/dbus-1/system.d
)

install(
	FILES
	"com.canonical.UserMetrics.conf"
	DESTINATION ${DBUSCONFDIR}
)

