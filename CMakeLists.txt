cmake_minimum_required(VERSION 3.15)
project(sfutils LANGUAGES CXX)

add_library(sfutils_base INTERFACE)
add_library(${PROJECT_NAME}::base ALIAS sfutils_base)
target_include_directories(sfutils_base INTERFACE "${PROJECT_SOURCE_DIR}")
target_compile_features(sfutils_base INTERFACE cxx_std_20)

add_subdirectory(sfutils_unicode)
