#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

#include "config.h"

class Motor
{

public:

    static void init();

    static void setMotorStatus( MotorStatus motorStatus );
    static void setMotorStatus( MotorStatus motorStatus, uint8_t speed );

private:

    static void setMotorStatusPins( MotorStatus motorStatus );
    static void setupPWMFrequency();

};

#endif // _MOTOR_H_

