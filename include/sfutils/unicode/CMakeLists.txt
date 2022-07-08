set(LIB_NAME ${PROJECT_NAME}_unicode)

add_library(${LIB_NAME} INTERFACE)
target_link_libraries(${LIB_NAME} INTERFACE ${PROJECT_NAME}::base)
target_link_libraries(${LIB_NAME} INTERFACE ${utf8proc_LIBRARIES})
target_include_directories(${LIB_NAME} INTERFACE ${utf8proc_INCLUDE_DIRS})
target_compile_options(${LIB_NAME} INTERFACE ${utf8proc_CFLAGS_OTHER})


set_target_properties(${LIB_NAME} PROPERTIES EXPORT_NAME unicode)
add_library(${PROJECT_NAME}::unicode ALIAS ${LIB_NAME})

install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
install(FILES encoding_cvt.hpp unicode_string.hpp usym_width.hpp
	ustring_iterator.hpp
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/unicode)

