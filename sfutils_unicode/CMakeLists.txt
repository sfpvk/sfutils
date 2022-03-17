find_package(Boost REQUIRED COMPONENTS locale)

add_library(sfutils_unicode INTERFACE)
add_library(${PROJECT_NAME}::unicode ALIAS sfutils_unicode)
target_link_libraries(sfutils_unicode INTERFACE Boost::locale)
target_link_libraries(sfutils_unicode INTERFACE sfutils::base)
