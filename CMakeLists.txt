cmake_minimum_required(VERSION 3.14)
project("app")

set(CMAKE_CXX_STANDARD 17)

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

set(SOURCES_LIST
	"utils/str.cpp"
	"app/app.cpp"
	"app/menu/menu.cpp"
	"app/menu/command_info.cpp"
	"main.cpp"
	"app/app_state.cpp"
	"core/location.cpp"
	"core/protocol.cpp"
	"app/menu/command_parser.cpp"
	"utils/print.cpp")

add_executable(${PROJECT_NAME} ${SOURCES_LIST})

# поиск заголовочных файлов а поддиректориях
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

set(LIBRARIES_LIST
	rang
	nlohmann_json)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES_LIST})

# cli11
