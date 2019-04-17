#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

#include "../config.h"

class Motor {

public:
    enum MotorCommand {
        Neutral = 0, Forward = 1, Backward = 2, Brake = 3
    };
    static const uint8_t SPEED_MAX = 100;


    explicit Motor(uint8_t pinPWM, uint8_t pinInA, uint8_t pinInB);

    void setCommand(MotorCommand motorStatus);
    void setCommand(MotorCommand motorStatus, uint8_t speed);

    void neutral();

private:
    static const uint8_t PWM_MAX = 255;

    uint8_t pinPWM;
    uint8_t pinInA;
    uint8_t pinInB;

    void setStatusPins(MotorCommand motorStatus);

};

#endif // _MOTOR_H_

