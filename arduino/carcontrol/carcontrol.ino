#include <Arduino.h>
#include <Servo.h>

#include <stdint.h>

#include "config.h"
#include "steering.h"
#include "motor.h"
#include "communication.h"

void setup();
void loop();
void dispatch(char *message, uint16_t length);

void setup()
{
    Communication::setup();
    Motor::setup();
    Steering::setup();
}

void loop()
{
    Communication::receive(&dispatch);
}

void dispatch(char *message, uint16_t length)
{
    message[1] = 42;
    Communication::send(message, length);
}

