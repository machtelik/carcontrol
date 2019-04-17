#include "car.h"
#include "Arduino.h"
#include "hardware/motor.h"
#include "hardware/steering.h"
#include "communication/communication.h"
#include "communication/message.h"

Car::Car() :
        motor(new Motor(PIN_MOTOR_PWM, PIN_MOTOR_INA, PIN_MOTOR_INB)),
        steering(new Steering(PIN_STEERING_SERVO)),
        communication(new Communication(SERIAL_SPEED)),
        lastMessageReceived(millis()) {

    communication->setMessageCallback(this);

}

Car::~Car() {
    delete motor;
    delete steering;
    delete communication;
}

void Car::drive() {

    communication->receive();

    if (millis() - lastMessageReceived > MESSAGE_TIMEOUT) {
        motor->neutral();
        steering->neutral();
    }
}

void Car::onNewMessage(const Message* message) {
    lastMessageReceived = millis();

    switch(message->type()) {

        case CARCONTROL_MESSAGE_TYPE:
            handleCarControlMessage(message);
            break;

        default:
            return;
    }
}

void Car::handleCarControlMessage(const Message* message) {
    if(message->type() != CARCONTROL_MESSAGE_TYPE || message->payloadSize() != CARCONTROL_MESSAGE_PAYLOAD_SIZE) {
        return;
    }

    auto payload = message->payload();

    uint8_t motorSpeed = payload[0];
    auto motorCommand = (Motor::MotorCommand) payload[1];
    motor->setCommand(motorCommand, motorSpeed);

    uint8_t steeringPosition = payload[2];
    steering->setPosition(steeringPosition);
}