
configure_file(
	"infographic-service-source.click-hook.in"
	"${CMAKE_SOURCE_DIR}/debian/infographic-service-source.click-hook"
	@ONLY
)

configure_file(
	"infographic-service-visualization.click-hook.in"
	"${CMAKE_SOURCE_DIR}/debian/infographic-service-visualization.click-hook"
	@ONLY
)

add_executable(visualizers-update visualizers-update.c)
set_target_properties(visualizers-update PROPERTIES OUTPUT_NAME "visualizers-update")
install(TARGETS visualizers-update RUNTIME DESTINATION "${pkglibexecdir}")

add_executable(sources-update sources-update.c)
set_target_properties(sources-update PROPERTIES OUTPUT_NAME "sources-update")
install(TARGETS sources-update RUNTIME DESTINATION "${pkglibexecdir}")
