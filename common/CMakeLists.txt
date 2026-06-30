# создание библиотеки
set(APP_LIB_NAME "common_lib")

set(SOURCES_LIST
	"utils/str/str.cpp"
	"network/protocol/protocol.cpp"
	"validator/validator.cpp"
	"utils/network/network.cpp"
	"network/network_address/network_address.cpp"
	"app/config/config/config.cpp"
	"app/menu/menu_item/menu_item_empty/menu_item_empty.cpp"
	"app/menu/menu_item/menu_item_invalid/menu_item_invalid.cpp"
	"app/menu/command_parser/command_parser.cpp"
	"network/socket/socket/socket.cpp"
	"app/menu/menu/menu.cpp"
	"app/signals/signal_handler/signal_handler.cpp"
	"network/binary_serializer/binary_serializer.cpp"
	"network/socket/socket_message_header/socker_message_header.cpp"
	"network/socket/socket_message/socket_message.cpp"
	"network/json_deserializer/json_deserializer.cpp"
	"app/menu/menu_item/menu_item_empty/menu_item_empty.cpp"
	"core/request/rrc_connection_request/rrc_connection_request.cpp"
	"core/request/request/request.cpp"
	"core/request/measurement_control_request/measurement_control_request.cpp"
	"core/request/measurement_report_request/measurement_report_request.cpp"
	"core/request/attach_accept_request/attach_accept_request.cpp"
	"core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.cpp"
	"core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.cpp"
	"core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.cpp"
	"core/request/error_request/error_request.cpp"
	"core/request/request_type/request_type.cpp"
	"network/binary_iterator/binary_iterator.cpp"
	"json/info/json_object_info/json_object_info.cpp"
	"json/info/json_object_array_info/json_object_array_info.cpp"
	"json/info/json_base_info/json_base_info.cpp"
	"core/request/empty_request/empty_request.cpp"
	"core/request/sm_transfer_request/sm_transfer_request.cpp"
	"core/request/sm_delivery_request/sm_delivery_request.cpp"
	"core/request/sm_delivery_report_request/sm_delivery_report_request.cpp"
	"core/request/sm_delivery_ack_request/sm_delivery_ack_request.cpp"
	"core/request/sm_delivery_error_request/sm_delivery_error_request.cpp"
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARIES_LIST})

if(BUILD_COMMON_TESTS)
    add_subdirectory(tests)
endif()

