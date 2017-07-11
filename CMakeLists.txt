cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

project(MessageHub VERSION 0.0.2)

set(MESSAGEHUB_VERSION_MAJOR 0)
set(MESSAGEHUB_VERSION_MINOR 0)
set(MESSAGEHUB_VERSION_PATCH 2)
set(MESSAGEHUB_VERSION_STRING ${MESSAGEHUB_VERSION_MAJOR}.${MESSAGEHUB_VERSION_MINOR}.${MESSAGEHUB_VERSION_PATCH})

set(LIB_NAME "messagehub")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzmq -std=c++11 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/thirdparty")

find_package(ZeroMQ REQUIRED)

# Set the include directory

execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# These are the include directories
set(INCLUDE_DIRS
    ./include/
    ${THIRDPARTY_DIR}/rapidjson/include/
    ${THIRDPARTY_DIR}/spdlog/include/
)

# These include directories are only for testing
# They are separate so they don't get installed
set(TEST_INCLUDE_DIRS
    ${THIRDPARTY_DIR}/googletest/googletest/include/
    ${THIRDPARTY_DIR}/googletest/googlemock/include/
)

include_directories(
    ${INCLUDE_DIRS}
    ${TEST_INCLUDE_DIRS}
)


link_directories(${CMAKE_BINARY_DIR})

add_subdirectory(src)


install(DIRECTORY ${INCLUDE_DIRS}  DESTINATION /usr/local/include)


add_custom_target(uninstall
                  COMMAND xargs rm < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
                  COMMENT "Uninstalling MessageHub")
