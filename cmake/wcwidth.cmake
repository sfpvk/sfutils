function(wcwidth_scope)
	set(LIB_NAME ${PROJECT_NAME}_wcwidth)
	if(NOT HAVE_WCWIDTH)
		set(SOURCE_ROOT ${PROJECT_SOURCE_DIR}/extern)
		set(INSTALL_HROOT ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/extern)

		add_library(${LIB_NAME} STATIC
			${SOURCE_ROOT}/wcwidth/wcwidth.c)
		target_include_directories(${LIB_NAME} INTERFACE 
			$<BUILD_INTERFACE:${SOURCE_ROOT}>
			$<INSTALL_INTERFACE:${INSTALL_HROOT}>)

		install(FILES ${SOURCE_ROOT}/wcwidth/wcwidth.h
			DESTINATION ${INSTALL_HROOT}/wcwidth)
	else()
		add_library(${LIB_NAME} INTERFACE)
	endif()
	set_target_properties(${LIB_NAME} PROPERTIES EXPORT_NAME wcwidth)
	add_library(${PROJECT_NAME}::wcwidth ALIAS ${LIB_NAME})
	install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
endfunction()
wcwidth_scope()
