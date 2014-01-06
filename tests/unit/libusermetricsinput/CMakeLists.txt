
set(TEST_HOME_DIR "/tmp/usermetrics-input-tests/")
set(TEST_CACHE_DIR "${TEST_HOME_DIR}/.cache")
add_definitions(-DTEST_HOME_DIR="${TEST_HOME_DIR}")
add_definitions(-DTEST_CACHE_DIR="${TEST_CACHE_DIR}")

set(
	USERMETRICSINPUT_UNIT_TESTS_SRC
	TestMetricImpl.cpp
	TestMetricManagerImpl.cpp
	TestUserMetricInputCAPI.cpp
)

add_executable(
	usermetricsinput-unit-tests
	${USERMETRICSINPUT_UNIT_TESTS_SRC}
)

target_link_libraries(
	usermetricsinput-unit-tests
	usermetricsinput-static
	usermetrics-test-utils
	${GTEST_LIBRARIES}
	${GMOCK_LIBRARIES}
)

add_valgrind_test(
	usermetricsinput-unit-tests
	usermetricsinput-unit-tests
)
