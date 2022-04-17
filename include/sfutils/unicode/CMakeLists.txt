set(LIB_NAME ${PROJECT_NAME}_unicode)

add_library(${LIB_NAME} INTERFACE)
target_link_libraries(${LIB_NAME} INTERFACE ${PROJECT_NAME}::base
	INTERFACE ${PROJECT_NAME}::cpp-unicodelib
	INTERFACE ${PROJECT_NAME}::wcwidth)
add_library(${PROJECT_NAME}::unicode ALIAS ${LIB_NAME})

install(TARGETS ${LIB_NAME} EXPORT ${EXPORT_FILE})
install(FILES encoding_cvt.hpp unicode_string.hpp usym_width.hpp
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/unicode)

