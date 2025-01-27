# Included CMakeLists for ISH source code
set(SOURCE_FILES
    ish.cpp
    command.cpp)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(ish ${SOURCE_FILES})
    target_link_libraries(ish ${Boost_LIBRARIES})
endif()
