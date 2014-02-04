

install(
	PROGRAMS helper-exec-tool.sh
	DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/upstart-app-launch/infographic-visualization/"
	RENAME "exec-tool"
)
