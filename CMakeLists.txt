# CMake Specification for C++ Version of the CS857 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.20)

project(UNM-CS887-ISH)

#add_subdirectory(src)
# Comment outthe line above and uncomment this instead if you want 
# to use the old C parser instead of the new C++ parser.
#add_subdirectory(src-c) 

enable_testing()
add_subdirectory(test)
