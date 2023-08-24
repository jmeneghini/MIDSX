project(pymidsx)
file(GLOB PYMIDSX_SOURCES
        ${CMAKE_SOURCE_DIR}/python/_midsx/*.cpp
        ${CMAKE_SOURCE_DIR}/python/_midsx/*.h
)

pybind11_add_module(pymidsx ${PYMIDSX_SOURCES})

# set folder for the .so file
set_target_properties(pymidsx PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python)

# link MCXrayTransport to the pybind11 module
target_link_libraries(pymidsx PRIVATE MCXrayTransport)