function(cpp_unicodelib_scope)
	set(LIB_NAME ${PROJECT_NAME}_cpp-unicodelib)
	set(SOURCE_ROOT ${PROJECT_SOURCE_DIR}/extern)
	set(INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/extern)

	add_library(${LIB_NAME} INTERFACE)
	target_include_directories(${LIB_NAME} INTERFACE 
		$<BUILD_INTERFACE:${SOURCE_ROOT}>
		$<INSTALL_INTERFACE:${INSTALL_ROOT}>)
	set_target_properties(${LIB_NAME} PROPERTIES EXPORT_NAME cpp-unicodelib)
	# /Zc:__cplusplus is required to make __cplusplus accurate
	# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
	# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
	# That version is equivalent to _MSC_VER==1914
	# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
	# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
	# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
	if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
		target_compile_options(${LIB_NAME} INTERFACE "/Zc:__cplusplus")
	endif()
	add_library(${PROJECT_NAME}::cpp-unicodelib ALIAS ${LIB_NAME})

	install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
	install(FILES ${SOURCE_ROOT}/cpp-unicodelib/unicodelib.h
		${SOURCE_ROOT}/cpp-unicodelib/unicodelib_encodings.h
		${SOURCE_ROOT}/cpp-unicodelib/unicodelib_names.h
		DESTINATION ${INSTALL_ROOT}/cpp-unicodelib)
endfunction()
cpp_unicodelib_scope()
