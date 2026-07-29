cmake_minimum_required(VERSION 3.14)
project("lte")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)

# установка библиотек
include(FetchContent)

fetchcontent_declare(
	rang
	URL https://github.com/agauniyal/rang/archive/refs/tags/v3.2.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(rang)

fetchcontent_declare(
	json
	URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(json)

fetchcontent_declare(
	cli11
	URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.6.2.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(cli11)

fetchcontent_declare(
	frozen
	URL https://github.com/serge-sans-paille/frozen/archive/refs/tags/1.2.0.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(frozen)

fetchcontent_declare(
    zpp_bits
    URL https://github.com/eyalz800/zpp_bits/archive/refs/tags/v4.7.1.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(zpp_bits)

add_library(zpp_bits INTERFACE)
target_include_directories(zpp_bits INTERFACE ${zpp_bits_SOURCE_DIR})

set(LIBRARIES_LIST
	rang
	nlohmann_json
	CLI11::CLI11
	frozen
	zpp_bits
)

add_subdirectory(common)

option(BUILD_CLIENT "Build client" OFF)
if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

option(BUILD_SERVER "Build server" OFF)
if(BUILD_SERVER)
    add_subdirectory(server)
endif()

option(BUILD_CLIENT_TESTS "Build client tests" OFF)
option(BUILD_COMMON_TESTS "Build common tests" OFF)

# создание библиотеки с утилитами для тестов
if(BUILD_CLIENT_TESTS OR BUILD_COMMON_TESTS)
	set(TEST_UTILS_SOURCES_LIST
		"test_utils/args_splitter/args_splitter.cpp"
		"test_utils/temp_file/temp_file.cpp"
	)
	add_library(test_utils STATIC ${TEST_UTILS_SOURCES_LIST})
endif()
