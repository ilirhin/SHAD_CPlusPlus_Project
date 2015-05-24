cmake_minimum_required(VERSION 2.8.4)
project(SHAD_CPlusPlus_Project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(HEADER_FILES)
set(SOURCE_FILES gamer_main.cpp)


add_executable(SHAD_CPlusPlus_Project ${SOURCE_FILES} ${HEADER_FILES} strategy_loader.h strategy_loader.cpp)