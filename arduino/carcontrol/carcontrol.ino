#include <Arduino.h>
#include <Servo.h>

#include <stdint.h>

class Steering;
class Motor;

#include "config.h"
#include "steering.h"
#include "motor.h"

void setup()
{
    Motor::init();
    Steering::init();
    Serial.begin( 9600 );
}

void loop()
{
    Motor::setMotorStatus( Forward, 20 );
    delay( 1000 );
}

