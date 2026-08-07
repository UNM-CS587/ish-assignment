#  CmakeLists for the testcases for ISH
cmake_host_system_information(RESULT host_name QUERY HOSTNAME)
message("-- _host_name variable is set to: " ${host_name})
set(HOME_DIR "$ENV{HOME}")

add_test(NAME BasicTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} basic.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(BasicTests
                  COMMAND ctest -R BasicTests)

add_test(NAME BasicErrors
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} basic-error.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(BasicErrors
                  COMMAND ctest -R BasicErrors)

add_test(NAME BuiltinTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} builtin.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME EnvironmentTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} environment.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME RedirectionTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} redirection.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME RedirectionErrors
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} redirection-error.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME CornerTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} corner.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME CornerTestsResourceLeak
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/resource-leak.sh $<TARGET_FILE:ish>
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME JobControlTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} job-control.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME JobControlTestsSignals
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/job-control-signals.sh $<TARGET_FILE:ish>
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME PipelineTests
         COMMAND shelltest -DPROMPT=${host_name}%
                           -DSHELL=$<TARGET_FILE:ish>
                           -DHOME=${HOME_DIR} pipeline.test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME PipelineTestsSignals
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pipeline-signals.sh $<TARGET_FILE:ish>
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
