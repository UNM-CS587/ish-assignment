# Included CMakeLists for ISH source code
set(SOURCE_FILES
    ish.cpp
    command.cpp)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70.0 REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
add_executable(ish ${SOURCE_FILES})
target_link_libraries(ish ${Boost_LIBRARIES})
