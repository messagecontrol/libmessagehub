cmake_minimum_required(VERSION 2.8)

project(MessageHub)

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
include_directories(
    ./include/    
    ./build/vendor/rapidjson/include/
    ./build/vendor/spdlog/include/
)

add_subdirectory(src)
link_directories(${CMAKE_BINARY_DIR})
add_library(messagehub ${SRCS})
target_link_libraries(messagehub  ${ZeroMQ_Library} )
add_executable(main src/main.cpp)
target_link_libraries(main messagehub)

install(TARGETS messagehub DESTINATION /usr/local/lib/messagehub)
install(DIRECTORY ./include/messagehub/ DESTINATION /usr/local/include)
      
add_custom_target(uninstall 
                  COMMAND xargs rm < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
                  COMMENT "Uninstalling MessageHub")
