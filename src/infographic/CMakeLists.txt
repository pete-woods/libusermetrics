
##########################
# Infographic
##########################

add_executable(
	infographic
	Infographic.cpp
	main.cpp
)

install(
	TARGETS infographic
	RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)

qt5_use_modules(
	infographic
	Core
)
