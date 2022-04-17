# Targets for external linking:
# sfutils::base    - (include files under <sfutils/base/...>)
# sfutils::unicode - (include files under <sfutils/unicode/...>)
cmake_minimum_required(VERSION 3.15)
project(sfutils)

option(ENABLE_TESTING "Enable Testing" OFF)

include(GNUInstallDirs)

set(EXPORT_FILE ${PROJECT_NAME}Config)
include(cmake/check_features.cmake)
include(cmake/common_options_and_procject_config.cmake)
include(cmake/cpp-unicodelib.cmake)
include(cmake/wcwidth.cmake)
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
