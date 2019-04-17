#include "steering.h"

#include <Arduino.h>
#include <Servo.h>

#include "../config.h"

Steering::Steering(uint8_t pinServo) :
        steeringServo(new Servo()) {

    steeringServo->attach(pinServo);
    neutral();

}

void Steering::neutral() {
    setPosition(0);
}

void Steering::setPosition(int8_t position) {

    position = min(position, MAX_STEER_OFFSET);
    position = max(position, -MAX_STEER_OFFSET);

    int8_t pos = NEUTRAL_OFFSET + position;

    steeringServo->write(pos);
}

int8_t Steering::position() const {
    return steeringServo->read() - NEUTRAL_OFFSET;
}

Steering::~Steering() {
    delete steeringServo;
}

