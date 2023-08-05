cmake_minimum_required(VERSION 3.10)

project(MCXRayTransport)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(MCXRayTransport ${SOURCES})

target_include_directories(MCXRayTransport PRIVATE ${INCLUDE_DIR})

include_directories(/usr/local/include/eigen3)

# Specify the Python interpreter
set(Python3_EXECUTABLE "/home/john/Documents/MCXrayTransport/env/bin/python")
find_package(Python3 3.10 EXACT REQUIRED COMPONENTS Interpreter Development)
include_directories(${Python3_INCLUDE_DIRS})

# Link the Python libraries
target_link_libraries(MCXRayTransport ${Python3_LIBRARIES})

# Find Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(MCXRayTransport ${Boost_LIBRARIES})
endif()

# Find SQLite3
find_library(SQLITE3_LIBRARY sqlite3)
if(NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 not found")
endif()
target_link_libraries(MCXRayTransport ${SQLITE3_LIBRARY})

## Find the NumPy libraries
#find_package(Python3 COMPONENTS NumPy)
#include_directories(${Python3_NumPy_INCLUDE_DIRS})

# Find the PyBind11 libraries
set(pybind11_DIR "/home/john/Documents/MCXrayTransport/env/lib/python3.10/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)
target_link_libraries(MCXRayTransport pybind11::module)

# Find matplot++ libraries
find_package(Matplot++ REQUIRED)
target_link_libraries(MCXRayTransport Matplot++::matplot)

message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

