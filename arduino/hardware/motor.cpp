#include "motor.h"

#include <Arduino.h>
#include "../config.h"


Motor::Motor(uint8_t pinPWM, uint8_t pinInA, uint8_t pinInB) :
        pinPWM(pinPWM),
        pinInA(pinInA),
        pinInB(pinInB) {

    pinMode(pinPWM, OUTPUT);
    pinMode(pinInA, OUTPUT);
    pinMode(pinInB, OUTPUT);

    neutral();
}

void Motor::setCommand(MotorCommand motorStatus) {
    setCommand(motorStatus, 0);
}

void Motor::setCommand(MotorCommand motorStatus, uint8_t speed) {
    setStatusPins(motorStatus);

    uint8_t pwm = 0;
    if (motorStatus == Forward || motorStatus == Backward) {
        pwm = PWM_MAX / SPEED_MAX * min(speed, SPEED_MAX);
    }

    analogWrite(pinPWM, pwm);
}

void Motor::neutral() {
    setCommand(Neutral, 0);
}

void Motor::setStatusPins(MotorCommand motorStatus) {
    switch (motorStatus) {
        case Neutral:
            digitalWrite(pinInA, LOW);
            digitalWrite(pinInB, LOW);
            break;

        case Forward:
            digitalWrite(pinInA, HIGH);
            digitalWrite(pinInB, LOW);
            break;

        case Backward:
            digitalWrite(pinInA, LOW);
            digitalWrite(pinInB, HIGH);
            break;

        case Brake:
            digitalWrite(pinInA, HIGH);
            digitalWrite(pinInB, HIGH);
            break;
    }
}
