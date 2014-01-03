
add_executable(tst_QmlTests tst_QmlTests.cpp)
qt5_use_modules(tst_QmlTests Core Qml Quick Test QuickTest)
target_link_libraries(tst_QmlTests ${TPL_QT5_LIBRARIES})

set(TEST_HOME_DIR "/tmp/usermetrics-qml-tests/")
set(TEST_CACHE_DIR "${TEST_HOME_DIR}/.cache")
add_definitions(-DTEST_CACHE_DIR="${TEST_CACHE_DIR}")

add_test(tst_QmlTests
	${CMAKE_CURRENT_BINARY_DIR}/tst_QmlTests
	-input ${CMAKE_CURRENT_SOURCE_DIR}
	-import ${SOURCE_BINARY_DIR}/modules
)
set_tests_properties(
	tst_QmlTests
	PROPERTIES
	ENVIRONMENT "QT_QPA_PLATFORM=minimal HOME=${TEST_HOME_DIR} APP_ID=test-app"
)

add_subdirectory(UserMetricsTest)
