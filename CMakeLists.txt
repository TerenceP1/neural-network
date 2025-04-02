cmake_minimum_required(VERSION 3.10)
project(setsolver)

add_executable(setsolver main.cpp)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(setsolver ${OpenCV_LIBS})

find_package(OpenCL REQUIRED)
target_link_libraries(setsolver OpenCL::OpenCL)
add_definitions(-DCL_TARGET_OPENCL_VERSION=300)