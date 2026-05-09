# создание библиотеки
set(APP_LIB_NAME common_lib)

set(SOURCES_LIST
	"utils/str.cpp"
	"core/location.cpp"
	"core/protocol.cpp"
	"utils/print.cpp"
	"validator/validator.cpp"
	"logging/logger.cpp"
	"utils/network.cpp"
	"core/network_address.cpp"
	"app/config/config.cpp"
)

add_library(${APP_LIB_NAME} STATIC ${SOURCES_LIST})

# поиск заголовочных файлов в поддиректориях
target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARIES_LIST})
