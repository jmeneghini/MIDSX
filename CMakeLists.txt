cmake_minimum_required(VERSION 3.10)
project(MIDSX VERSION 0.1.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(AddGitSubmodule.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(MIDSX_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SOURCE_DIR src/MIDSX)
set(INCLUDE_DIR include/MIDSX/)
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(MIDSX STATIC ${SOURCES})

find_package(OpenMP REQUIRED)
target_link_libraries(MIDSX PUBLIC OpenMP::OpenMP_CXX)

target_include_directories(MIDSX PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Eigen3 submodule
add_git_submodule(${CMAKE_CURRENT_LIST_DIR}/extern/eigen)
target_link_libraries(MIDSX PUBLIC Eigen3::Eigen)

# Specify the Python interpreter
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Python3 found with target Python3::Python")
    target_link_libraries(MIDSX PUBLIC Python3::Python)
else()
    message(WARNING "Python3 not found")
endif()

# Pybind11 submodule
add_git_submodule(${CMAKE_CURRENT_LIST_DIR}/extern/pybind11)
target_link_libraries(MIDSX PUBLIC pybind11::embed)


# Find SQLite3
find_library(SQLITE3_LIBRARY sqlite3)
if(NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 not found")
endif()
target_link_libraries(MIDSX PUBLIC ${SQLITE3_LIBRARY})

configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/MIDSXConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/MIDSXConfig.cmake
        INSTALL_DESTINATION lib/cmake/MIDSX
)

# Set db install directory
set(MIDSX_DB_DIR ${CMAKE_INSTALL_PREFIX}/share/MIDSX)

# Configure the config file
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/include/MIDSX/Core/config.h.in
        ${CMAKE_CURRENT_LIST_DIR}/include/MIDSX/Core/config.h
        @ONLY
)



# Installation and export
install(TARGETS MIDSX
        EXPORT MIDSXTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# install db
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/data_sources/EPDL/midsx.db
        DESTINATION ${MIDSX_DB_DIR})


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
