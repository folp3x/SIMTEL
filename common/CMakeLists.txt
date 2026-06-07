# создание библиотеки
set(APP_LIB_NAME "common_lib")

set(SOURCES_LIST
	"utils/str/str.cpp"
	"network/protocol/protocol.cpp"
	"validator/validator.cpp"
	"logging/logger/logger.cpp"
	"utils/network/network.cpp"
	"network/network_address/network_address.cpp"
	"app/config/config/config.cpp"
	"app/menu/menu_item/menu_item_exit/menu_item_exit.cpp"
	"app/menu/menu_item/menu_item_invalid/menu_item_invalid.cpp"
	"app/menu/command_parser/command_parser.cpp"
	"network/socket/socket/socket.cpp"
	"app/menu/menu/menu.cpp"
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

target_compile_definitions(${APP_LIB_NAME} PUBLIC
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARIES_LIST})

if(BUILD_COMMON_TESTS)
    add_subdirectory(tests)
endif()

