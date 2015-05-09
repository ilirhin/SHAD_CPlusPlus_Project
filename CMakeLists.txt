cmake_minimum_required(VERSION 2.8.4)
project(SHAD_CPlusPlus_Project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADER_FILES client.h)
set(SOURCE_FILES angry_balls.cpp)


add_executable(SHAD_CPlusPlus_Project ${SOURCE_FILES} ${HEADER_FILES})