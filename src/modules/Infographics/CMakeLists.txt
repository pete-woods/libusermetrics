include(Plugins)

set(plugin_SRCS
	Components.cpp
)

include_directories(
	${Qt5Quick_INCLUDE_DIRS}
)

add_library(
	Infographics-qml
	SHARED
	${plugin_SRCS}
)

qt5_use_modules(
	Infographics-qml
	Qml
	Quick
)

target_link_libraries(
	Infographics-qml
	usermetricsoutput
)

export_qmlfiles(
	Infographics
	PLUGIN_SUBPATH "Infographics"
	PLUGIN_PATH ${CMAKE_INSTALL_LIBDIR}/unity8/qml
)

export_qmlplugin(
	Infographics 0.1
	TARGETS Infographics-qml
	PLUGIN_SUBPATH "Infographics"
	PLUGIN_PATH ${CMAKE_INSTALL_LIBDIR}/unity8/qml
)
