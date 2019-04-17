#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

#define PIN_STEERING_SERVO  5
#define PIN_MOTOR_PWM 6
#define PIN_MOTOR_INA 8
#define PIN_MOTOR_INB 7

static const uint8_t MAX_MESSAGE_SIZE = 256;
static const uint16_t SERIAL_SPEED = 115200;

#endif // _CONFIG_H_
