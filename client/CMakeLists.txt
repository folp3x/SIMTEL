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
	"core/address_book/address_book_parser/address_book_parser.cpp"
	"app/menu/menu_item/menu_item_sms/menu_item_sms.cpp"
	"app/menu/menu_item/menu_item_sent/menu_item_sent.cpp"
	"app/menu/menu_item/menu_item_received/menu_item_received.cpp"
	"app/menu/menu_item/menu_item_dialog/menu_item_dialog.cpp"
	"validator/validator.cpp"
	"core/ue/ue_context/ue_context.cpp"
	"core/ue/ue_exchange/ue_exchange.cpp"
	"app/menu/menu_item/menu_item_exit/menu_item_exit.cpp"
)

set(CLIENT_LIBRARIES_LIST
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

if(BUILD_CLIENT_TESTS)
    add_subdirectory(tests)
endif()
