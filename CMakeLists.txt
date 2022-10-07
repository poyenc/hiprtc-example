project(hiprtc-example)

cmake_minimum_required(VERSION 3.10.2)

set (CMAKE_CXX_STANDARD 17)

set(HIP_INSTALL_PREFIX "/opt/rocm/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(include)

add_subdirectory(src)
