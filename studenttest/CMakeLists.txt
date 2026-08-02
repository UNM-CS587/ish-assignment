#  CmakeLists for student testcases for ISH

find_package(Boost 1.70.0 REQUIRED)
find_package(GTest REQUIRED)

add_executable(parser_test
    parser_test.cpp
    ${CMAKE_SOURCE_DIR}/src/command.cpp)
target_include_directories(parser_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src)
target_include_directories(parser_test SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS})
target_link_libraries(parser_test PRIVATE GTest::gtest_main)

add_test(NAME StudentTests COMMAND parser_test)
