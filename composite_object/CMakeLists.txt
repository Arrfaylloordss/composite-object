add_library (composite_object INTERFACE)

target_sources (composite_object INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/composite_object.hpp
)

target_include_directories (composite_object INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})