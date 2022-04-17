function(cpp_unicodelib_scope)
	set(LIB_NAME ${PROJECT_NAME}_cpp-unicodelib)
	set(SOURCE_ROOT ${PROJECT_SOURCE_DIR}/extern)
	set(INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/extern)

	add_library(${LIB_NAME} INTERFACE)
	target_include_directories(${LIB_NAME} INTERFACE 
		$<BUILD_INTERFACE:${SOURCE_ROOT}>
		$<INSTALL_INTERFACE:${INSTALL_ROOT}>)
	add_library(${PROJECT_NAME}::cpp-unicodelib ALIAS ${LIB_NAME})

	install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
	install(FILES ${SOURCE_ROOT}/cpp-unicodelib/unicodelib.h
		${SOURCE_ROOT}/cpp-unicodelib/unicodelib_encodings.h
		${SOURCE_ROOT}/cpp-unicodelib/unicodelib_names.h
		DESTINATION ${INSTALL_ROOT}/cpp-unicodelib)
endfunction()
cpp_unicodelib_scope()
