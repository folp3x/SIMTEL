cmake_minimum_required(VERSION 3.14)
project("app")

set(CMAKE_CXX_STANDARD 23)

# установка библиотек
include(FetchContent)

FetchContent_Declare(
	rang
	URL https://github.com/agauniyal/rang/archive/refs/tags/v3.2.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(rang)

FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
	cli11
	URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.6.2.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(
	spdlog
	URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(spdlog)

set(LIBRARIES_LIST
	rang
	nlohmann_json
	CLI11::CLI11
	spdlog
)

add_subdirectory(common)

option(BUILD_CLIENT "Build client" OFF)
if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

# флаг для сборки с тестами
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
