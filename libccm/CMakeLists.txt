project(ccm)

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(communication)
add_subdirectory(util)
add_subdirectory(eventloop)

SET(SRC_LIST
  ${SRC_LIST}
  ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread)