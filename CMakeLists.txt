cmake_minimum_required (VERSION 3.4)

set (CMAKE_CONFIGURATION_TYPES Debug;Release)

project(CompositeObject CXX)

# if (WIN32)
#     if (MSVC)
#         string (REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#         string (REPLACE "/MDd" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
#         string (REPLACE "/MD"  "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
#         string (REPLACE "/MD"  "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
#         set (CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /DUNICODE /D_UNICODE /MP")
#         set (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#         set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#         set (CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} /MTd")
#         set (CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /MT /O2")
#     endif (MSVC)
# endif (WIN32)

# message (STATUS "C++ compiler flags:         ${CMAKE_CXX_FLAGS}")
# message (STATUS "C++ compiler flags DEBUG:   ${CMAKE_CXX_FLAGS_DEBUG}")
# message (STATUS "C++ compiler flags RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
if (WITH_TESTS)
    add_definitions(-DWITH_TESTS)
    enable_testing()
endif ()

add_subdirectory (composite_object)

if (WITH_TESTS)
    add_subdirectory (test)
endif ()