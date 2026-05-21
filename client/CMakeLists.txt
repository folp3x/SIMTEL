set(APP_TARGET_NAME "client") 
set(APP_LIB_NAME "client_lib")

set(SOURCES_LIST
	"app/app/app.cpp"
	"app/menu/menu/menu.cpp"
	"app/menu/command_parser/command_parser.cpp"
	"app/cli/cli_parser/cli_parser.cpp"
	"app/config/config_parser/config_parser.cpp"
	"app/config/config/config.cpp"
	"app/menu/menu_item/menu_item_active/menu_item_active.cpp"
	"app/menu/menu_item/menu_item_protocol/menu_item_protocol.cpp"
	"network/socket/socket.cpp"
)

set(CLIENT_LIBRARIES_LIST
	common_lib
)

# создание библиотеки
add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${CLIENT_LIBRARIES_LIST})

# создание исполняемого файла
add_executable(${APP_TARGET_NAME} "main.cpp")
target_link_libraries(${APP_TARGET_NAME} PRIVATE ${APP_LIB_NAME})

option(COPY_CLIENT_DATA "Copy data directory" ON)

if(COPY_CLIENT_DATA)
add_custom_command(TARGET ${APP_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/data" 
        "$<TARGET_FILE_DIR:${APP_TARGET_NAME}>/data"
)
endif()

if(BUILD_CLIENT_TESTS)
    add_subdirectory(tests)
endif()
