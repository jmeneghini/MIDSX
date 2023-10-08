cmake_minimum_required(VERSION 3.10)
project(distributions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") # turn on OpenMP

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
endfunction()

list(APPEND CMAKE_PREFIX_PATH "/home/john/Documents/MCXrayTransport/env/lib/python3.10/site-packages/pybind11/share/cmake")
find_package(pybind11 REQUIRED)
find_package(MIDSX REQUIRED)

set(COMMON_LIBS pybind11::embed MIDSX::MIDSX)

create_executable(distributions distributions.cpp)
create_executable(source_ang_dist source_ang_dist.cpp)
