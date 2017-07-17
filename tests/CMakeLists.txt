cmake_minimum_required(VERSION 2.8)

set(TEST_SRCS
    test_manager.cpp
)

add_executable(test_manager ${TEST_SRCS})

target_link_libraries(test_manager gtest gtest_main)
target_link_libraries(test_manager messagehub)



add_test(test-manager test_manager)

set(TEST_EXECUTABLES
    ./test_manager
)

add_custom_target(check 
                  COMMAND pwd && ${PROJECT_SOURCE_DIR}/tests/run_tests.sh ${TEST_EXECUTABLES}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
