cmake_minimum_required(VERSION 3.8)
project(spas_fract_168_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB CPP_FILES ./*.cpp)
file(GLOB HEADER_FILES ./*.hpp)

add_executable(main ${CPP_FILES} ${HEADER_FILES})