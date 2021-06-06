cmake_minimum_required(VERSION 3.19)
project(Black_white_FileSystem C)

set(CMAKE_C_STANDARD 11)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
include_directories(.)

file(GLOB SRC "main.c")
add_executable(Black_white_FileSystem ${SRC})

target_link_libraries(Black_white_FileSystem PUBLIC ${FUSE_LIBRARIES})
target_include_directories(Black_white_FileSystem PUBLIC ${FUSE_INCLUDE_DIRS})



