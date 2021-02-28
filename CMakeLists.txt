cmake_minimum_required(VERSION 3.17)
project(Matt_daemon)

set(CMAKE_CXX_STANDARD 20)

include_directories(Includes)
add_executable(Matt_daemon Sources/main.cpp Sources/Daemon.cpp Includes/Daemon.hpp)