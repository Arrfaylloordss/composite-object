add_executable (composite_object_test main.cpp)

target_link_libraries (composite_object_test composite_object)

add_test (NAME cmake_composite_object_test COMMAND composite_object_test)