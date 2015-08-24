#include <Arduino.h>
#include <Servo.h>

#include <stdint.h>

class Steering;
class Motor;
class Communication;

#include "config.h"
#include "steering.h"
#include "motor.h"
#include "communication.h"

void setup();
void loop();
void dispatch(char *message, uint16_t length);

void setup()
{
    Communication::init();
    Motor::init();
    Steering::init();
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

