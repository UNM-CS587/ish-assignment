# CMake Specification for C++ Version of the CS587 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.20)

project(UNM-CS587-ISH)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

# The provided parser uses Boost::Spirit to parse command lines. These are set
# here rather than in src/ because CMAKE_CXX_STANDARD is directory-scoped: set
# further down, the shell would build as C++20 while the unit tests in
# studenttest/ built as the compiler default.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# README.md requires code that compiles without warnings, so students need to
# be able to see them.
add_compile_options(-Wall -Wextra)

add_subdirectory(src)

add_subdirectory(report)

enable_testing()
add_subdirectory(classtest)
add_subdirectory(studenttest)

