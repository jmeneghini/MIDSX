cmake_minimum_required(VERSION 3.10)
project(MIDSX VERSION 0.1.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenMP REQUIRED)

set(SOURCE_DIR src/MIDSX)

set(INCLUDE_DIR include/MIDSX/)

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(MIDSX STATIC ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Release build. Parallelization enabled.")
    find_package(OpenMP REQUIRED)
    target_link_libraries(MIDSX PUBLIC OpenMP::OpenMP_CXX)
endif()

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



# Specify the Python interpreter and link pybind11
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Python3 found with target Python3::Python")

    # Use the Python interpreter to get the pybind11 path
    execute_process(
            COMMAND ${Python3_EXECUTABLE} -c
            "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Use the path from the previous command to find the package
    list(APPEND CMAKE_PREFIX_PATH ${PYBIND11_CMAKE_DIR})
    find_package(pybind11 REQUIRED)

    target_link_libraries(MIDSX PUBLIC Python3::Python)
    target_link_libraries(MIDSX PUBLIC pybind11::embed)
else()
    message(WARNING "Python3 not found")
endif()


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
target_link_libraries(MIDSX PUBLIC ${SQLITE3_LIBRARY})

# Find the PyBind11 libraries
#find_package(pybind11 REQUIRED)
#target_link_libraries(MIDSX PRIVATE pybind11::embed)


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
        DESTINATION include/MIDSX)

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
