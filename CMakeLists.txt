cmake_minimum_required(VERSION 2.8)

project(MessageHub)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzmq -std=c++14 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ZeroMQ REQUIRED)

# Set the include directory

include_directories(
    ./include/    
)

add_subdirectory(src)
link_directories(${CMAKE_BINARY_DIR})
add_library(messagehub STATIC ${SRCS})
target_link_libraries(messagehub LINK_PUBLIC ${ZeroMQ_Library} )
add_executable(main src/main.cpp)
target_link_libraries(main ${ZeroMQ_Library} messagehub)

