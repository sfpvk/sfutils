set(LIB_NAME ${PROJECT_NAME}_base)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_link_libraries(${LIB_NAME}
	INTERFACE ${PROJECT_NAME}::common_options)
add_library(${PROJECT_NAME}::base ALIAS ${LIB_NAME})

install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
install(FILES base.hpp
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/base)
