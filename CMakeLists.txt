cmake_minimum_required(VERSION 3.10)
project(MIDSX VERSION 0.1.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") # turn on OpenMP
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") # turn on optimization
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_DIR src)
set(INCLUDE_DIR include)

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(MIDSX STATIC ${SOURCES})

target_include_directories(MIDSX PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


find_package(Eigen3 REQUIRED)
if(TARGET Eigen3::Eigen)
    message(STATUS "Eigen3 found with target Eigen3::Eigen")
    target_link_libraries(MIDSX PUBLIC Eigen3::Eigen)
else()
    message(WARNING "Eigen3 does not provide the expected target Eigen3::Eigen")
endif()



# Specify the Python interpreter
set(Python3_EXECUTABLE "/home/john/Documents/MCXrayTransport/env/bin/python")
find_package(Python3 3.10 EXACT REQUIRED COMPONENTS Interpreter Development)
target_include_directories(MIDSX PRIVATE ${Python3_INCLUDE_DIRS})

# Find Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(MIDSX PRIVATE ${Boost_LIBRARIES})
endif()

# Find SQLite3
find_library(SQLITE3_LIBRARY sqlite3)
if(NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 not found")
endif()
target_link_libraries(MIDSX PRIVATE ${SQLITE3_LIBRARY})

# Find the PyBind11 libraries
list(APPEND CMAKE_PREFIX_PATH "/home/john/Documents/MCXrayTransport/env/lib/python3.10/site-packages/pybind11/share/cmake")
find_package(pybind11 REQUIRED)
target_link_libraries(MIDSX PRIVATE pybind11::embed)


configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/MIDSXConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/MIDSXConfig.cmake
        INSTALL_DESTINATION lib/cmake/MIDSX
)

# Installation and export
install(TARGETS MIDSX
        EXPORT MIDSXTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


install(DIRECTORY ${INCLUDE_DIR}/
        DESTINATION include)

install(EXPORT MIDSXTargets
        FILE MIDSXTargets.cmake
        NAMESPACE MIDSX::
        DESTINATION lib/cmake/MIDSX)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        MIDSXConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MIDSXConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/MIDSXConfigVersion.cmake
        DESTINATION lib/cmake/MIDSX)
