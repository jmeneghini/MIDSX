cmake_minimum_required(VERSION 3.10)
project(pymidsx)
set(CMAKE_CXX_FLAGS "-g -O0") # debug, no optimization

file(GLOB PYMIDSX_SOURCES
        ${CMAKE_SOURCE_DIR}/_pymidsx/*.cpp
        ${CMAKE_SOURCE_DIR}/_pymidsx/*.h
)

list(APPEND CMAKE_PREFIX_PATH "/home/john/Documents/MCXrayTransport/env/lib/python3.10/site-packages/pybind11/share/cmake")
find_package(pybind11 REQUIRED)

pybind11_add_module(pymidsx ${PYMIDSX_SOURCES})

# link MIDSX to the pybind11 module
find_package(MIDSX REQUIRED)
target_link_libraries(pymidsx PUBLIC MIDSX::MIDSX)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(pymidsx PUBLIC OpenMP::OpenMP_CXX)
endif()


# set folder for the .so file
set_target_properties(pymidsx PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

target_link_libraries(pymidsx PRIVATE pybind11::module)
