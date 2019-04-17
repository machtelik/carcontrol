cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/libraries/cmake/Arduino-Toolchain.cmake)
set(PROJECT_NAME arduino)

project(${PROJECT_NAME} LANGUAGES C CXX ASM)

get_board_id(board_id nano atmega328)

find_arduino_library(servo_lib Servo ${board_id} 3RD_PARTY)

add_arduino_executable(${PROJECT_NAME} ${board_id} main.cpp hardware/motor.cpp hardware/steering.cpp communication/communication.cpp communication/message.cpp car.cpp)
link_arduino_library(${PROJECT_NAME} servo_lib ${board_id})

#upload_arduino_target(${PROJECT_NAME} "${board_id}" /dev/ttyACM0)

