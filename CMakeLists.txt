cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

project(MessageHub VERSION 0.2.0)

set(MESSAGEHUB_VERSION_MAJOR 0)
set(MESSAGEHUB_VERSION_MINOR 2)
set(MESSAGEHUB_VERSION_PATCH 0)
set(MESSAGEHUB_VERSION_STRING ${MESSAGEHUB_VERSION_MAJOR}.${MESSAGEHUB_VERSION_MINOR}.${MESSAGEHUB_VERSION_PATCH})

set(LIB_NAME "messagehub")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzmq -std=c++14 -pthread")
#set(BUILD_SHARED_LIBS off)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

find_package(ZeroMQ REQUIRED)

# Set the include directory

execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# These are the include directories
# The vendor directories are hard coded because they will always
# be created those directories, no matter the build location
set(INCLUDE_DIRS
    ./include/
    ./build/vendor/rapidjson/include/
    ./build/vendor/spdlog/include/
 )
include_directories(
    ./include/    
    ./build/vendor/rapidjson/include/
    ./build/vendor/spdlog/include/
)


add_subdirectory(src)
link_directories(${CMAKE_BINARY_DIR})
add_library(messagehub SHARED ${SRCS})
set_target_properties(messagehub PROPERTIES VERSION ${MESSAGEHUB_VERSION_STRING}
                                                    SOVERSION ${MESSAGEHUB_VERSION_STRING}
                                                    OUTPUT_NAME ${LIB_NAME}
)
add_library(messagehub_static STATIC ${SRCS})
set_target_properties(messagehub_static PROPERTIES VERSION ${MESSAGEHUB_VERSION_STRING}
                                                    SOVERSION ${MESSAGEHUB_VERSION_STRING}
                                                    OUTPUT_NAME ${LIB_NAME}
)

target_link_libraries(messagehub  ${ZeroMQ_Library} )
target_link_libraries(messagehub_static  ${ZeroMQ_Library} )
add_executable(main src/main.cpp)
target_link_libraries(main messagehub)


install(TARGETS messagehub messagehub_static DESTINATION /usr/local/lib/messagehub)

install(DIRECTORY ${INCLUDE_DIRS}  DESTINATION /usr/local/include)


add_custom_target(uninstall 
                  COMMAND xargs rm < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
                  COMMENT "Uninstalling MessageHub")
