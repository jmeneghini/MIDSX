cmake_minimum_required(VERSION 3.10)
project(radiography)

function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
endfunction()

# MIDSX and any other libraries that you want to link to
find_package(MIDSX REQUIRED)

set(COMMON_LIBS MIDSX::MIDSX)

create_executable(radiography radiography.cpp)
create_executable(radiography_15_degrees radiography_15_degrees.cpp)
