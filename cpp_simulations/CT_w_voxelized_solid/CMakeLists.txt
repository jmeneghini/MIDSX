cmake_minimum_required(VERSION 3.10)
project(CT_w_voxelized_solid)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} ${COMMON_LIBS})
endfunction()

find_package(MIDSX REQUIRED)

# MIDSX and any other libraries that you want to link to
set(COMMON_LIBS MIDSX::MIDSX)

create_executable(CT_w_voxelized_solid CT_w_voxelized_solid.cpp)
