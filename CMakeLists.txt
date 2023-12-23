
cmake_minimum_required(VERSION 3.20)
project(frq)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

add_executable(frq_text main.cpp frq.hpp)
