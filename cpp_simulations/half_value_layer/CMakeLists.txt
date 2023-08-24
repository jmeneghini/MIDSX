cmake_minimum_required(VERSION 3.10)
project(hvl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") # turn on OpenMP


add_executable(hvl hvl.cpp)

list(APPEND CMAKE_PREFIX_PATH "/home/john/Documents/MCXrayTransport/env/lib/python3.10/site-packages/pybind11/share/cmake")
find_package(pybind11 REQUIRED)
target_link_libraries(hvl PRIVATE pybind11::embed)

find_package(MIDSX REQUIRED)
target_link_libraries(hvl PUBLIC MIDSX::MIDSX)
