find_package(Git REQUIRED)
message(STATUS "Using Git at: ${GIT_EXECUTABLE}")


function(add_git_submodule dir)
# add a Git submodule directory to CMake, assuming the
# Git submodule directory is a CMake project.
#
# Usage: in CMakeLists.txt
# 
# include(AddGitSubmodule.cmake)
# add_git_submodule(mysubmod_dir)

if(NOT EXISTS ${dir}/CMakeLists.txt)
  message("Git submodule ${dir} does not exist, initializing it")
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- "${dir}"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)
endif()

add_subdirectory(${dir})

endfunction(add_git_submodule)
