include(Plugins)

set(plugin_SRCS
    Components.cpp
    Metric.cpp
)

include_directories(
    ${Qt5Quick_INCLUDE_DIRS}
)

add_library(
	usermetrics-qml
	SHARED ${plugin_SRCS}
)

target_link_libraries(
	usermetrics-qml
	usermetricsinput
)

qt5_use_modules(
	usermetrics-qml
	Qml
	Quick
)

export_qmlfiles(
	UserMetrics
	PLUGIN_SUBPATH "UserMetrics"
)

export_qmlplugin(
	UserMetrics 0.1
	PLUGIN_SUBPATH "UserMetrics"
	TARGETS usermetrics-qml
)
