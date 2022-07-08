# Targets for external linking:
# sfutils::base    - (include files under <sfutils/base/...>)
# sfutils::unicode - (include files under <sfutils/unicode/...>)
cmake_minimum_required(VERSION 3.22)
project(sfutils)

option(ENABLE_TESTING "Enable Testing" OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(utf8proc REQUIRED libutf8proc)

include(GNUInstallDirs)

set(EXPORT_FILE ${PROJECT_NAME}Config)

include(cmake/common_options.cmake)
add_subdirectory(include/sfutils/base)
add_subdirectory(include/sfutils/unicode)

if(ENABLE_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()

install(EXPORT ${EXPORT_FILE}
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
export(EXPORT ${EXPORT_FILE} NAMESPACE ${PROJECT_NAME}::)
