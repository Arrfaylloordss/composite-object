cmake_minimum_required (VERSION 3.4)

set (CMAKE_CONFIGURATION_TYPES Debug;Release)

project(CompositeObject CXX)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++1y)
elseif (NOT MSVC)
    add_compile_options(-std=c++1y)
endif()

if (WITH_TESTS)
    add_definitions(-DWITH_TESTS)
    enable_testing()
endif ()

add_subdirectory (composite_object)

if (WITH_TESTS)
    add_subdirectory (test)
endif ()