set(TESTS_TARGET_NAME "common_tests")

enable_testing()

# установка библиотек
include(FetchContent)

FetchContent_Declare(
	gtest
	URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(gtest)

set(TESTS_SOURCES_LIST
	"main_test.cpp"
	"test_validator.cpp"
	"test_location.cpp"
	"test_network_address.cpp"
)

add_executable(${TESTS_TARGET_NAME} ${TESTS_SOURCES_LIST})

set(TESTS_LIBRARIES_LIST
	GTest::gtest_main
	test_utils
	common_lib
)

target_link_libraries(${TESTS_TARGET_NAME} PRIVATE ${TESTS_LIBRARIES_LIST})
