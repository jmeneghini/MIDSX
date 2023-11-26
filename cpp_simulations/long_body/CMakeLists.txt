cmake_minimum_required(VERSION 3.10)
project(long_body)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
endfunction()

find_package(pybind11 REQUIRED)
find_package(MIDSX REQUIRED)
find_package(OpenMP REQUIRED)

set(COMMON_LIBS pybind11::embed MIDSX::MIDSX)

create_executable(long_body long_body.cpp)
