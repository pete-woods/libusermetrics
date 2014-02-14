
configure_file(
	"usermetrics-source.click-hook.in"
	"${CMAKE_SOURCE_DIR}/debian/usermetrics-source.click-hook"
	@ONLY
)
add_executable(visualizers-update visualizers-update.cpp)
install(
	TARGETS visualizers-update
	RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)

configure_file(
	"usermetrics-visualization.click-hook.in"
	"${CMAKE_SOURCE_DIR}/debian/usermetrics-visualization.click-hook"
	@ONLY
)
add_executable(sources-update sources-update.cpp)
install(
	TARGETS sources-update
	RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/libusermetrics"
)
