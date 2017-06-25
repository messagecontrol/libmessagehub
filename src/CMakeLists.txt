cmake_minimum_required(VERSION 2.8)

# Since the executables will be made in the parent CMakeLists.txt
# The variables need to be in the parent scope, so set the filenames
# accordingly.

set(SRCS
    src/messagehub.cpp
    src/message.cpp 

    PARENT_SCOPE
)


