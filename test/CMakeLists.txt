add_executable(ustring ustring.cpp)
target_link_libraries(ustring PRIVATE sfutils::unicode)
add_test(ustring_test ustring)
