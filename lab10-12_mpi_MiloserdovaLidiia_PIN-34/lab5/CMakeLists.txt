cmake_minimum_required(VERSION 3.23)
project(mpi-lab-5)

include_directories(/usr/include/x86_64-linux-gnu/mpi/)

set(CMAKE_CXX_STANDARD 20)

add_executable(mpi-lab-5 main.cpp)
