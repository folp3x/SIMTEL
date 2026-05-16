set(APP_TARGET_NAME "server") 
set(APP_LIB_NAME "server_lib")

set(SOURCES_LIST
	"app/cli/cli_parser/cli_parser.cpp"
    "validator/validator.cpp"
    "app/app/app.cpp"
    "app/menu/command_parser/command_parser.cpp"
    "app/menu/menu/menu.cpp"
    "core/distance_calculator/distance_calculator.cpp"
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

option(COPY_SERVER_DATA "Copy data directory" ON)

if(COPY_SERVER_DATA)
add_custom_command(TARGET ${APP_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/data" 
        "$<TARGET_FILE_DIR:${APP_TARGET_NAME}>/data"
)
endif()
