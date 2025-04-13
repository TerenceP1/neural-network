cmake_minimum_required(VERSION 3.10)
project(ml)

add_executable(ml main.cpp)


find_package(OpenCL REQUIRED)
target_link_libraries(ml OpenCL::OpenCL)
add_definitions(-DCL_TARGET_OPENCL_VERSION=300)