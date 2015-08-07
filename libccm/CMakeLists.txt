project(ccm)

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(SRC_LIST
  component.cpp
  multicastcommunication.cpp
  serialcommunication.cpp
  communication.cpp
  message.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread)