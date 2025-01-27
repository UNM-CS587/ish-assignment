# Included CMakeLists for ISH source code
set(SOURCE_FILES
    ish.cpp)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(ish ${SOURCEFILES} ish.cpp)
    target_link_libraries(ish ${Boost_LIBRARIES})
endif()
