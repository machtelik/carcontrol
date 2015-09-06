#include <Arduino.h>
#include <Servo.h>

#include <stdint.h>

#include "config.h"
#include "steering.h"
#include "motor.h"
#include "communication.h"
#include "message.h"

void setup();
void loop();
void dispatch(char *message, uint16_t length);
void handleCarControlMessage(char *message);

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
    if (length != Message::getMessageSize(message)) {
        return;
    }
    
    switch(Message::getType(message)) {
        
        case CARCONTROL_DATA_TYPE:
            handleCarControlMessage(message);
            break;
        
        default:
            return;
    }
}

void handleCarControlMessage(char *message)
{
    if(Message::getPayloadSize(message) != CARCONTROL_DATA_PAYLOAD_SIZE) {
        return;
    }
    
    char *payload = Message::getPayload(message);
    
    uint8_t speed = payload[0];
    MotorStatus motorStatus = (MotorStatus) payload[1];
    uint8_t steering = payload[2];
}
