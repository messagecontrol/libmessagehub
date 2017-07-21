cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

project(MessageHub VERSION 0.0.2)

option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)
option(BUILD_TESTS "Build test programs" ON)

set(MESSAGEHUB_VERSION_MAJOR 0)
set(MESSAGEHUB_VERSION_MINOR 0)
set(MESSAGEHUB_VERSION_PATCH 2)
set(MESSAGEHUB_VERSION_STRING ${MESSAGEHUB_VERSION_MAJOR}.${MESSAGEHUB_VERSION_MINOR}.${MESSAGEHUB_VERSION_PATCH})

set(LIB_NAME "messagehub")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzmq -std=c++14")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/thirdparty")

find_package(ZeroMQ REQUIRED)
find_package(Threads REQUIRED)

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
    ${PROJECT_SOURCE_DIR}/tests/fixtures/
)


include_directories(
    ${INCLUDE_DIRS}
    ${TEST_INCLUDE_DIRS}
)


link_directories(${CMAKE_BINARY_DIR})

add_subdirectory(src)

if (BUILD_TESTS)
    set(SAVED_BUILD_SHARED_LIBS ${BUILD_STATIC_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(${THIRDPARTY_DIR}/googletest)
    enable_testing()
    add_subdirectory(tests)
    set(BUILD_SHARED_LIBS ${SAVED_BUILD_SHARED_LIBS})
endif (BUILD_TESTS)


install(DIRECTORY ${INCLUDE_DIRS}  DESTINATION /usr/local/include)


add_custom_target(uninstall
                  COMMAND xargs rm < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
                  COMMENT "Uninstalling MessageHub")
