function(common_options_and_procject_config_scope)
	set(LIB_NAME ${PROJECT_NAME}_common_options)
	set(INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/project_config)

	add_library(${LIB_NAME} INTERFACE)
	target_compile_features(${LIB_NAME} INTERFACE cxx_std_20)
	target_include_directories(${LIB_NAME} INTERFACE 
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
		$<INSTALL_INTERFACE:${INSTALL_ROOT}>)
	set_target_properties(${LIB_NAME} PROPERTIES EXPORT_NAME common_options)
	add_library(${PROJECT_NAME}::common_options ALIAS ${LIB_NAME})
	install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})

	configure_file(${PROJECT_SOURCE_DIR}/cmake/sfutils_config.h.in
		sfutils_config.h)
	install(FILES ${PROJECT_BINARY_DIR}/sfutils_config.h
		DESTINATION ${INSTALL_ROOT})
endfunction()
common_options_and_procject_config_scope()
