find_program(qmlplugindump_exe qmlplugindump)

if(NOT qmlplugindump_exe)
  msg(FATAL_ERROR "Could not locate qmlplugindump.")
endif()

function(QUERY_QMAKE VAR RESULT) 
    exec_program(${QMAKE_EXECUTABLE} ARGS "-query ${VAR}" RETURN_VALUE return_code OUTPUT_VARIABLE output ) 
    if(NOT return_code) 
        file(TO_CMAKE_PATH "${output}" output) 
        set(${RESULT} ${output} PARENT_SCOPE) 
    endif(NOT return_code) 
endfunction(QUERY_QMAKE) 

# Creates target for copying and installing qmlfiles
#
# export_qmlfiles(plugin sub_path)
#
#
# Target to be created:
#   - plugin-qmlfiles - Copies the qml files (*.qml, *.js, qmldir) into the shadow build folder.

macro(export_qmlfiles PLUGIN)
    set(one_value_args PLUGIN_SUBPATH PLUGIN_PATH)
	CMAKE_PARSE_ARGUMENTS(_ARG "" "${one_value_args}" "" ${ARGN})

    file(GLOB QMLFILES
        *.qml
        *.js
        qmldir
    )

    # copy the qmldir file
    add_custom_target(${PLUGIN}-qmlfiles ALL
                        COMMAND cp ${QMLFILES} ${CMAKE_CURRENT_BINARY_DIR}
                        DEPENDS ${QMLFILES}
    )

    if(NOT _ARG_PLUGIN_PATH)
        # Qt5's cmake does not export QT_IMPORTS_DIR, lets query qmake on our own for now
        get_target_property(QMAKE_EXECUTABLE Qt5::qmake LOCATION) 
        query_qmake(QT_INSTALL_QML _ARG_PLUGIN_PATH)
    endif()

    if(_ARG_PLUGIN_SUBPATH)
        set(PLUGIN_DESTINATION "${_ARG_PLUGIN_PATH}/${_ARG_PLUGIN_SUBPATH}")
    else()
        set(PLUGIN_DESTINATION "${_ARG_PLUGIN_PATH}")
    endif()

    # install the qmlfiles file.
    install(FILES ${QMLFILES}
        DESTINATION ${PLUGIN_DESTINATION}
    )
endmacro(export_qmlfiles)


# Creates target for generating the qmltypes file for a plugin and installs plugin files
#
# export_qmlplugin(plugin version sub_path [TARGETS target1 [target2 ...]])
#
# TARGETS additional install targets (eg the plugin shared object)
#
# Target to be created:
#   - plugin-qmltypes - Generates the qmltypes file in the shadow build folder.

macro(export_qmlplugin PLUGIN VERSION)
    set(one_value_args PLUGIN_SUBPATH PLUGIN_PATH)
    set(multi_value_keywords TARGETS)
    cmake_parse_arguments(_ARG "" "${one_value_args}" "${multi_value_keywords}" ${ARGN})

    if(NOT _ARG_PLUGIN_PATH)
        # Qt5's cmake does not export QT_IMPORTS_DIR, lets query qmake on our own for now
        get_target_property(QMAKE_EXECUTABLE Qt5::qmake LOCATION) 
        query_qmake(QT_INSTALL_QML _ARG_PLUGIN_PATH)
    endif()

    if(_ARG_PLUGIN_SUBPATH)
        set(PLUGIN_DESTINATION "${_ARG_PLUGIN_PATH}/${_ARG_PLUGIN_SUBPATH}")
    else()
        set(PLUGIN_DESTINATION "${_ARG_PLUGIN_PATH}")
    endif()

    # Only try to generate .qmltypes if not cross compiling
    if(NOT CMAKE_CROSSCOMPILING)
        # create the plugin.qmltypes file
        add_custom_target(${PLUGIN}-qmltypes ALL
            COMMAND ${qmlplugindump_exe} -notrelocatable ${PLUGIN} ${VERSION} ${CMAKE_CURRENT_BINARY_DIR}/../ > ${CMAKE_CURRENT_BINARY_DIR}/plugin.qmltypes
        )
        add_dependencies(${PLUGIN}-qmltypes ${PLUGIN}-qmlfiles ${_ARG_TARGETS})

        # install the qmltypes file.
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugin.qmltypes
            DESTINATION ${PLUGIN_DESTINATION}
        )
    endif()

    # install the additional targets
    install(TARGETS ${_ARG_TARGETS}
        DESTINATION ${PLUGIN_DESTINATION}
    )
endmacro(export_qmlplugin)
