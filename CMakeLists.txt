cmake_minimum_required(VERSION 3.10)

project(MCXRayTransport)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(MCXRayTransport ${SOURCES})

target_include_directories(MCXRayTransport PRIVATE ${INCLUDE_DIR})

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
