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
	"network/json_deserializer/json_deserializer.cpp"
	"app/menu/menu_item/menu_item_empty/menu_item_empty.cpp"
	"core/request/rrc_connection_request/rrc_connection_request.cpp"
	"core/request/request/request.cpp"
	"core/response/measurement_control_response/measurement_control_response.cpp"
	"core/request/measurement_report_request/measurement_report_request.cpp"
	"core/response/attach_accept_response/attach_accept_response.cpp"
	"core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.cpp"
	"core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.cpp"
	"core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.cpp"
	"core/response/error_response/error_response.cpp"
	"core/request/request_type/request_type.cpp"
	"network/binary_iterator/binary_iterator.cpp"
	"json/info/json_object_info/json_object_info.cpp"
	"json/info/json_object_array_info/json_object_array_info.cpp"
	"json/info/json_base_info/json_base_info.cpp"
	"core/request/empty_request/empty_request.cpp"
	"core/request/sm_transfer_request/sm_transfer_request.cpp"
	"core/response/sm_delivery_response/sm_delivery_response.cpp"
	"core/response/sm_delivery_report_response/sm_delivery_report_response.cpp"
	"core/request/sm_delivery_ack_request/sm_delivery_ack_request.cpp"
	"core/response/sm_delivery_error_response/sm_delivery_error_response.cpp"
	"core/request/ussd_code_request/ussd_code_request.cpp"
	"core/response/ussd_balance_response/ussd_balance_response.cpp"
	"core/response/ussd_msisdn_response/ussd_msisdn_response.cpp"
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARIES_LIST})

if(BUILD_COMMON_TESTS)
    add_subdirectory(tests)
endif()

