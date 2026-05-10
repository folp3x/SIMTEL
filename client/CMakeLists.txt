set(APP_TARGET_NAME client) 
# создание библиотеки
set(APP_LIB_NAME client_lib)

set(SOURCES_LIST
	"app/app.cpp"
	"menu/menu/menu.cpp"
	"menu/command_parser/command_parser.cpp"
	"cli/cli_parser/cli_parser.cpp"
	"config/config_parser/config_parser.cpp"
	"config/config/config.cpp"
	"menu/menu_item/menu_item.cpp"
)

set(CLIENT_LIBRARIES_LIST
	common_lib
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${CLIENT_LIBRARIES_LIST})

# создание исполняемого файла
add_executable(${APP_TARGET_NAME} "main.cpp")
target_link_libraries(${APP_TARGET_NAME} PRIVATE ${APP_LIB_NAME})

# копирование директории data
add_custom_command(TARGET ${APP_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/data" 
        "$<TARGET_FILE_DIR:${APP_TARGET_NAME}>/data"
)
