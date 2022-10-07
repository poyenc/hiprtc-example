find_package(hip REQUIRED)

include_directories("${HIP_INSTALL_PREFIX}/include")

add_definitions(-D__HIP_PLATFORM_AMD__)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result")

add_library(get_kernel SHARED get_kernel.cpp)
add_library(utility utility.cpp)
add_executable(client client.cpp)

target_link_libraries(client
  hip::device
  get_kernel
  utility
)

install(FILES kernel.cu DESTINATION ${CMAKE_BINARY_DIR})
