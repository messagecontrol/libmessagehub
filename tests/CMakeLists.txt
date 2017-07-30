cmake_minimum_required(VERSION 2.8)

find_package(ZeroMQ REQUIRED)
find_package(Threads REQUIRED)

set(TEST_SRCS
        test_message.cpp
        test_messagecontrol.cpp
)


add_executable(test_messagecontrol test_messagecontrol.cpp)
add_executable(test_message test_message.cpp)

message(STATUS ${ZeroMQ_Library})
target_link_libraries(test_messagecontrol gtest gtest_main ${cppzmq_LIBRARY} messagehub)
target_link_libraries(test_message gtest gtest_main ${cppzmq_LIBRARY} messagehub)


add_test(test-messagecontrol test_messagecontrol)
add_test(test-message test_message)

set(TEST_EXECUTABLES
    ./test_messagecontrol
    ./test_message
)

add_custom_target(check 
                  COMMAND pwd && ${PROJECT_SOURCE_DIR}/tests/run_tests.sh ${TEST_EXECUTABLES}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
