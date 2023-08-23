cmake_minimum_required(VERSION 3.10)
project(hvl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") # turn on OpenMP

add_executable(hvl hvl.cpp)

find_package(MIDSX REQUIRED)
target_link_directories(hvl PUBLIC ${MIDSX_LIBRARY_DIRS})
