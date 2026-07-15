set(APP_TARGET_NAME "server") 
set(APP_LIB_NAME "server_lib")

set(SOURCES_LIST
    "app/app/app.cpp"
    "app/menu/menu/menu.cpp"
    "app/message_holder/message_holder.cpp"
    "app/config/config/config.cpp"
    "app/config/config_parser/config_parser.cpp"
    "app/config/bs_config/bs_config_parser/bs_config_parser.cpp"
    "app/config/epc_config/epc_config_parser/epc_config_parser.cpp"

    "network/socket/socket.cpp"

    "core/simtel/simtel_listener/simtel_listener.cpp"
    "core/simtel/simtel_ue_context/simtel_ue_context.cpp"
    "core/simtel/simtel_base_station/simtel_base_station.cpp"
    "core/simtel/simtel_mme/simtel_mme.cpp"
    "core/simtel/simtel_smsc/simtel_smsc.cpp"
    "core/simtel/simtel_visitor_list/simtel_visitor_list.cpp"
    "core/simtel/simtel_register/simtel_register.cpp"
    "core/ttl_manager/ttl_manager.cpp"
    "core/simtel/vlr_record/vlr_record.cpp"
    "core/simtel/subscriber_status/subscriber_status.cpp"
    "core/simtel/simtel_pcrf/simtel_pcrf.cpp"
)

# установка библиотек
include(FetchContent)

fetchcontent_declare(
    sqlite_orm
    URL https://github.com/fnc12/sqlite_orm/archive/refs/tags/v1.9.1.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(sqlite_orm)

find_package(SQLite3 REQUIRED)

set(CLIENT_LIBRARIES_LIST
    SQLite::SQLite3
    sqlite_orm
	common_lib
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${CLIENT_LIBRARIES_LIST})

add_executable(${APP_TARGET_NAME} "main.cpp")
target_link_libraries(${APP_TARGET_NAME} PRIVATE ${APP_LIB_NAME})

add_custom_command(TARGET ${APP_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/data" 
        "$<TARGET_FILE_DIR:${APP_TARGET_NAME}>/data"
)
