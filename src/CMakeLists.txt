
include(GenerateExportHeader)

include_directories(${SOURCE_DIR})
include_directories(${SOURCE_BINARY_DIR})

add_compiler_export_flags()

add_subdirectory("libusermetricscommon")
add_subdirectory("libusermetricsinput")
add_subdirectory("libusermetricsoutput")
add_subdirectory("usermetricsservice")
add_subdirectory("modules")
