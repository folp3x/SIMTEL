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
	"utils/print.cpp"
	"app/validator/validator.cpp"
	"app/cli/cli_parser.cpp"
	"app/config/config_parser.cpp"
	"app/config/config.cpp"
	"logging/logger.cpp"
	"app/menu/menu_item.cpp"
	"utils/network.cpp"
	"core/network_address.cpp"
	"utils/vec.cpp")

add_executable(${PROJECT_NAME} ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

set(LIBRARIES_LIST
	rang
	nlohmann_json
	CLI11::CLI11
	spdlog)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES_LIST})

# копирование директории data
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/data" 
        "$<TARGET_FILE_DIR:app>/data"
)

# копирование директории data/incorrect_configs
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/data/incorrect_configs" 
        "$<TARGET_FILE_DIR:app>/data/incorrect_configs"
)
