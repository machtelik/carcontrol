#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

#define PIN_STEERING_SERVO  5
#define PIN_MOTOR_PWM 6
#define PIN_MOTOR_INA 8
#define PIN_MOTOR_INB 7

static const float STEERING_SERVO_NEUTRAL = 90;
static const float STEERING_SERVO_MAX_DEVIATION = 45;
static const int8_t STEERING_DEVIATION = 100;

static const int8_t MAX_MOTOR_SPEED = 100;
static const uint8_t MOTOR_MAX_PWM = 255;

static const uint16_t SERIAL_SPEED = 115200;

static const uint16_t MESSAGE_HEADER_SIZE = 4; //bytes
static const uint16_t MESSAGE_MAX_SIZE = 512; //bytes

static const uint8_t MESSAGE_START = 0;
static const uint8_t MESSAGE_END = 1;
static const uint8_t MESSAGE_ESCAPE = 2;
static const uint8_t MESSAGE_START_ESCAPED = 3;
static const uint8_t MESSAGE_END_ESCAPED = 4;
static const uint8_t MESSAGE_ESCAPE_ESCAPED = 5;

enum MotorStatus { Neutral = 0, Forward = 1, Backward = 2, Brake = 3 };

static const uint8_t CARCONTROL_DATA_TYPE = 0;
static const uint8_t CARCONTROL_DATA_PAYLOAD_SIZE = 3;

#endif // _CONFIG_H_
