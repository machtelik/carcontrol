add_subdirectory(message)
add_subdirectory(types)

SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/communication.cpp
    PARENT_SCOPE
)