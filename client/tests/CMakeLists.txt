set(TESTS_TARGET_NAME "client_tests")

enable_testing()

# установка библиотек
include(FetchContent)

fetchcontent_declare(
	gtest
	URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(gtest)

set(TESTS_SOURCES_LIST
	"main_test.cpp"
	"test_cli_parser.cpp"
	"test_config_parse.cpp"
	"test_command_parser.cpp"
)

add_executable(${TESTS_TARGET_NAME} ${TESTS_SOURCES_LIST})

set(TESTS_LIBRARIES_LIST
	GTest::gtest_main
	test_utils
	client_lib
)

target_link_libraries(${TESTS_TARGET_NAME} PRIVATE ${TESTS_LIBRARIES_LIST})
