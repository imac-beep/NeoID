cmake_minimum_required(VERSION 2.8)
project(NeoID C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=gnu99 -Werror")

set(SOURCE_FILES src/nid_core.h src/neoid.c src/nid_genid.c src/nid_server.c src/nid_event.c)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/bin)
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
SET(EXECUTABLE_OUTPUT_PATH ${dir}/bin)
SET(LIBRARY_OUTPUT_PATH ${dir}/lib)
SET(CMAKE_CACHEFILE_DIR ${dir})

add_executable(neoid ${SOURCE_FILES})
