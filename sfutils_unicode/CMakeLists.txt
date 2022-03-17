find_package(Boost REQUIRED COMPONENTS locale)

add_library(sfutils_unicode INTERFACE)
add_library(${PROJECT_NAME}::unicode ALIAS sfutils_unicode)
target_link_libraries(sfutils_unicode INTERFACE Boost::locale)
target_include_directories(sfutils_unicode INTERFACE "${PROJECT_SOURCE_DIR}")
target_compile_features(sfutils_unicode INTERFACE cxx_std_20)
