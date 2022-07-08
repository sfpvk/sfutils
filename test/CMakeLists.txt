add_executable(ustring ustring.cpp)
target_link_libraries(ustring PRIVATE sfutils::unicode)
add_test(ustring_test ustring)

add_executable(wcwidth wcwidth.cpp)
target_link_libraries(wcwidth PRIVATE sfutils::unicode)
add_test(wcwidth_test wcwidth)

add_executable(encoding_cvt encoding_cvt.cpp)
target_link_libraries(encoding_cvt PRIVATE sfutils::unicode)
add_test(encoding_cvt_test encoding_cvt)
