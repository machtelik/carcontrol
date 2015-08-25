#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

#include "config.h"

namespace Motor
{

    void setup();

    void setMotorStatus( MotorStatus motorStatus );
    void setMotorStatus( MotorStatus motorStatus, uint8_t speed );

};

#endif // _MOTOR_H_

