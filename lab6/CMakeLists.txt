cmake_minimum_required(VERSION 3.24)
project(mpi_lab_6)

include_directories(/usr/include/x86_64-linux-gnu/mpi/)

set(CMAKE_CXX_STANDARD 20)

add_executable(mpi_lab_6 ring.cpp star.cpp)
