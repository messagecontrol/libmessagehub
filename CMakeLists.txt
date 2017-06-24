cmake_minimum_required(VERSION 2.8)

project(MessageHub)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


find_package(ZeroMQ REQUIRED)

# Set the include directory

include_directories(
    ./include/    
)

# Get the source files
# This will also give the following variables
# MESSAGEHUB_SRCS (Not Yet)
# RECEIVER_SRCS   (Not Yet)
# SENDER_SRCS     (Not Yet)
add_subdirectory(src)

set(SOURCES 
    ${MESSAGEHUB_SRCS}
    ${RECEIVER_SRCS}
    ${SENDER_SRCS} 
)

message("Sources: " ${SOURCES})
# Create Executable
add_executable(MessageHub ${SOURCES})
target_link_libraries(MessageHub ${ZeroMQ_LIBRARY})
