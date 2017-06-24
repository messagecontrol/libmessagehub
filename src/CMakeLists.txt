cmake_minimum_required(VERSION 2.8)

# Since the executables will be made in the parent CMakeLists.txt
# The variables need to be in the parent scope, so set the filenames
# accordingly.

set(MESSAGEHUB_SRCS
    src/messagehub.cpp
    PARENT_SCOPE    
)

set(RECEIVER_SRCS
    
)

set(SENDER_SRCS
    
)
