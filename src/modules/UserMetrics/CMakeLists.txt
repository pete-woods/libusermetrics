set(plugin_SRCS
    Components.cpp
    Metric.cpp
)

set(plugin_HDRS
    Components.h
    Metric.h
)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

add_library(usermetrics-qml SHARED ${plugin_SRCS} ${plugin_HDRS})

target_link_libraries(usermetrics-qml
    usermetricsinput
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)

set(QT_IMPORTS_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/qml")
set(PLUGIN_DIR ${QT_IMPORTS_DIR}/UserMetrics)
install(TARGETS usermetrics-qml DESTINATION ${PLUGIN_DIR})
install(FILES qmldir DESTINATION ${PLUGIN_DIR})

# copy qmldir file to build dir so QML unit tests can use it to import the plugin
configure_file(
	qmldir
	qmldir
)
