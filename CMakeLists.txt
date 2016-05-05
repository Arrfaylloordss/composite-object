cmake_minimum_required (VERSION 3.4)

set (CMAKE_CONFIGURATION_TYPES Debug;Release)

project(CompositeObject CXX)

if (WITH_TESTS)
    add_definitions(-DWITH_TESTS)
    enable_testing()
endif ()

add_subdirectory (composite_object)

if (WITH_TESTS)
    add_subdirectory (test)
endif ()