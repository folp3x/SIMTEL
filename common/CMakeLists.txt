# создание библиотеки
set(APP_LIB_NAME "common_lib")

set(SOURCES_LIST
	"src/common/utils/str/str.cpp"
	"src/common/utils/network/network.cpp"

	"src/common/validator/validator.cpp"

	"src/common/network/protocol/protocol.cpp"
	"src/common/network/network_address/network_address.cpp"
	"src/common/network/socket/socket/socket.cpp"
	"src/common/network/network_error/network_error_type/network_error_type.cpp"

	"src/common/app/config/config/config.cpp"
	"src/common/app/menu/menu_item/menu_item_empty/menu_item_empty.cpp"
	"src/common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.cpp"
	"src/common/app/menu/command_parser/command_parser.cpp"
	"src/common/app/menu/menu/menu.cpp"
	"src/common/app/signal_handler/signal_handler.cpp"
	"src/common/app/menu/menu_item/menu_item_empty/menu_item_empty.cpp"

	"src/common/core/request/rrc_connection_request/rrc_connection_request.cpp"
	"src/common/core/request/request/request.cpp"
	"src/common/core/request/measurement_report_request/measurement_report_request.cpp"
	"src/common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.cpp"
	"src/common/core/request/ussd_code_request/ussd_code_request.cpp"
	"src/common/core/request/request_type/request_type.cpp"
	"src/common/core/request/empty_request/empty_request.cpp"
	"src/common/core/request/sm_transfer_request/sm_transfer_request.cpp"
	"src/common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.cpp"

	"src/common/core/response/measurement_control_response/measurement_control_response.cpp"
	"src/common/core/response/attach_accept_response/attach_accept_response.cpp"
	"src/common/core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.cpp"
	"src/common/core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.cpp"
	"src/common/core/response/error_response/error_response.cpp"
	"src/common/core/response/sm_delivery_response/sm_delivery_response.cpp"
	"src/common/core/response/sm_delivery_report_response/sm_delivery_report_response.cpp"
	"src/common/core/response/sm_delivery_error_response/sm_delivery_error_response.cpp"
	"src/common/core/response/ussd_balance_response/ussd_balance_response.cpp"
	"src/common/core/response/ussd_msisdn_response/ussd_msisdn_response.cpp"

	"src/common/json/info/json_object_info/json_object_info.cpp"
	"src/common/json/info/json_repeat_object_info/json_repeat_object_info.cpp"
	"src/common/json/json_parsable/json_parsable.cpp"

	"src/common/binary/binary_serializable/binary_serializable.cpp"
	"src/common/binary/binary_serializer/binary_serializer.cpp"
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(${APP_LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARIES_LIST})

if(BUILD_COMMON_TESTS)
    add_subdirectory(tests)
endif()

