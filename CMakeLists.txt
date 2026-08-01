# CMake Specification for C++ Version of the CS857 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.20)

project(UNM-CS887-ISH)

add_subdirectory(src)

# Comment out this line if you're Patrick testing his implementation 
# (not provided) versus the testcases.
#add_subdirectory(sample)

enable_testing()
add_subdirectory(test)
