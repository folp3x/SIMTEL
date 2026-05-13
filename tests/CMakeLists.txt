set(TESTS_TARGET_NAME app_tests) 

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
	"client/test_cli_parser.cpp"
	"utils/temp_file/temp_file.cpp"
	"utils/args_splitter/args_splitter.cpp"
	"client/test_config_parser.cpp"
	"client/test_command_parser.cpp"
	"common/test_validator.cpp"
	"common/test_location.cpp"
	"common/test_network_address.cpp"
)

add_executable(${TESTS_TARGET_NAME} ${TESTS_SOURCES_LIST})

# поиск заголовочных файлов
target_include_directories(${TESTS_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

set(TESTS_LIBRARIES_LIST
	GTest::gtest_main
	client_lib
)

target_link_libraries(${TESTS_TARGET_NAME} PRIVATE ${TESTS_LIBRARIES_LIST})
