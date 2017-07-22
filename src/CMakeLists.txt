cmake_minimum_required(VERSION 2.8)

set(SRCS
    ${PROJECT_SOURCE_DIR}/src/messagecontrol.cpp
    ${PROJECT_SOURCE_DIR}/src/manager.cpp
    ${PROJECT_SOURCE_DIR}/src/message.cpp
)

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

target_link_libraries(messagehub  ${ZeroMQ_LIBRARY} )
target_link_libraries(messagehub_static  ${ZeroMQ_STATIC_LIBRARY} )

install(TARGETS messagehub messagehub_static DESTINATION /usr/local/lib/messagehub)