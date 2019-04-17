//
// Created by mike on 17.04.19.
//

#ifndef ARDUINO_CAR_H
#define ARDUINO_CAR_H

#include <stdint.h>
#include "communication/communication.h"

class Motor;
class Steering;
class Communication;
class Message;

class Car : Communication::MessageCallback {

public:
    explicit Car();
    virtual ~Car();

    void drive();

private:
    static const uint32_t MESSAGE_TIMEOUT = 500;
    static const uint8_t CARCONTROL_MESSAGE_TYPE = 0;
    static const uint8_t CARCONTROL_MESSAGE_PAYLOAD_SIZE = 3;

    Motor *motor;
    Steering *steering;
    Communication *communication;

    uint32_t lastMessageReceived;

    void onNewMessage(const Message* message) override;
    void handleCarControlMessage(const Message* message);
};


#endif //ARDUINO_CAR_H
