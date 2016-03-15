cmake_minimum_required (VERSION 2.8)

# Project
project (GameBoy-Emulator)
set (CXX "-std=c++11")
set (CMAKE_CXX_FLAGS "${CXX} -Wall")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")
aux_source_directory (. SRC_LIST)
add_executable (${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME} "SDL2")
