cmake_minimum_required(VERSION 2.8)

set(TEST_SRCS
    test_message.cpp
    test_manager.cpp
)


add_executable(test_manager test_manager.cpp)
add_executable(test_message test_message.cpp)

target_link_libraries(test_manager gtest gtest_main messagehub)
target_link_libraries(test_message gtest gtest_main messagehub)


add_test(test-manager test_manager)
add_test(test-message test_message)

set(TEST_EXECUTABLES
    ./test_manager
    ./test_message
)

add_custom_target(check 
                  COMMAND pwd && ${PROJECT_SOURCE_DIR}/tests/run_tests.sh ${TEST_EXECUTABLES}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
