cmake_minimum_required(VERSION 3.10)
project(CT_w_voxelized_solid)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
endfunction()


# add submodules (root/extern)
add_subdirectory(../../extern/pybind11 pybind11)

find_package(MIDSX REQUIRED)
find_package(OpenMP REQUIRED)


set(COMMON_LIBS pybind11::embed MIDSX::MIDSX)

create_executable(CT_w_voxelized_solid CT_w_voxelized_solid.cpp)
