cmake_minimum_required(VERSION 3.10)
project(radiography)

find_package(OpenMP REQUIRED)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
    if(CMAKE_BUILD_TYPE MATCHES Release)
        message(STATUS "Release build. Parallelization enabled.")
        target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)
    endif()
endfunction()

find_package(pybind11 REQUIRED)
find_package(MIDSX REQUIRED)

set(COMMON_LIBS pybind11::embed MIDSX::MIDSX)

create_executable(radiography radiography.cpp)
create_executable(radiography_test radiography_test.cpp)
create_executable(radiography_15_degrees radiography_15_degrees.cpp)
