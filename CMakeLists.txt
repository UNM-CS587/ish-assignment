# CMake Specification for C++ Version of the CS857 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(UNM-CS887-ISH)

# The provided parser uses Boost::Spirit to parse command lines

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.81.0) 

add_subdirectory(src)

