#  CmakeLists for the testcases for ISH
#
# Each shelltest category runs through run-shelltest.sh, which copies the
# fixtures into a scratch directory, resolves the prompt and HOME at test time,
# and bounds how long shelltest may run. See that script for why.
#
# Note for students: `ctest -R` matches substrings, not whole names, so
# `-R CornerTests` also runs CornerTestsResourceLeak, `-R JobControlTests` also
# runs JobControlTestsSignals, and `-R PipelineTests` also runs
# PipelineTestsSignals. The autograder grades each of those pairs as one
# category. See README.md.

set(SHELLTEST_RUNNER ${CMAKE_CURRENT_SOURCE_DIR}/run-shelltest.sh)

# A shelltest category. NEEDS_NONROOT marks the ones that assert a
# permission-denied error, which root cannot produce.
function(add_shelltest_category name testfile)
    cmake_parse_arguments(ARG "NEEDS_NONROOT" "" "" ${ARGN})
    if(ARG_NEEDS_NONROOT)
        set(nonroot_flag --needs-nonroot)
    else()
        set(nonroot_flag "")
    endif()

    add_test(NAME ${name}
             COMMAND ${SHELLTEST_RUNNER}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     $<TARGET_FILE:ish>
                     ${testfile}
                     ${nonroot_flag})
    # Well under the autograder's per-step budget, which is in minutes, so a
    # hung shell reports as a timeout with its output rather than burning the
    # whole step.
    set_tests_properties(${name} PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
    add_custom_target(${name} COMMAND ctest -R ${name})
endfunction()

# A script-driven category, for checks that inspect the running shell and so
# can't be expressed as shelltest input/output matching.
function(add_script_category name script)
    add_test(NAME ${name}
             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${script} $<TARGET_FILE:ish>
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(${name} PROPERTIES TIMEOUT 300)
    add_custom_target(${name} COMMAND ctest -R ${name})
endfunction()

add_shelltest_category(BasicTests        basic.test)
# basic-error.test needs no NEEDS_NONROOT: execve denies a mode-000 file even
# to root, since it requires at least one execute bit to be set.
add_shelltest_category(BasicErrors       basic-error.test)
add_shelltest_category(BuiltinTests      builtin.test)
add_shelltest_category(EnvironmentTests  environment.test)
add_shelltest_category(RedirectionTests  redirection.test NEEDS_NONROOT)
add_shelltest_category(RedirectionErrors redirection-error.test NEEDS_NONROOT)
add_shelltest_category(CornerTests       corner.test)
add_shelltest_category(JobControlTests   job-control.test)
add_shelltest_category(PipelineTests     pipeline.test)

add_script_category(CornerTestsResourceLeak resource-leak.sh)
add_script_category(JobControlTestsSignals  job-control-signals.sh)
add_script_category(PipelineTestsSignals    pipeline-signals.sh)
