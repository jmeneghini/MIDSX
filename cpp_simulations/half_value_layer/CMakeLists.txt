cmake_minimum_required(VERSION 3.10)
project(hvl)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
endfunction()

find_package(MIDSX REQUIRED)

# MIDSX and any other libraries that you want to link to
set(COMMON_LIBS MIDSX::MIDSX)

create_executable(hvl_30keV hvl_30keV.cpp)
create_executable(hvl_100keV hvl_100keV.cpp)
create_executable(hvl_30kVp hvl_30kVp.cpp)
create_executable(hvl_100kVp hvl_100kVp.cpp)


