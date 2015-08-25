#include "motor.h"

#include <Arduino.h>
#include "config.h"

namespace Motor {

void setMotorStatusPins( MotorStatus motorStatus );
void setupPWMFrequency();

void setup()
{
    pinMode( PIN_MOTOR_PWM, OUTPUT );
    pinMode( PIN_MOTOR_INA, OUTPUT );
    pinMode( PIN_MOTOR_INB, OUTPUT );

    setMotorStatus( Neutral );
}

void setMotorStatus( MotorStatus motorStatus )
{
    setMotorStatus( motorStatus, 0 );
}

void setMotorStatus( MotorStatus motorStatus, uint8_t speed )
{
    setMotorStatusPins( motorStatus );

    uint8_t pwm = 0;
    if( motorStatus == Forward || motorStatus == Backward ) {
        pwm = MOTOR_MAX_PWM / MAX_MOTOR_SPEED * min( speed, MAX_MOTOR_SPEED );
    }

    analogWrite( PIN_MOTOR_PWM, pwm );
}

void setMotorStatusPins( MotorStatus motorStatus )
{
    switch( motorStatus ) {
    case Neutral:
        digitalWrite( PIN_MOTOR_INA, LOW );
        digitalWrite( PIN_MOTOR_INB, LOW );
        break;

    case Forward:
        digitalWrite( PIN_MOTOR_INA, HIGH );
        digitalWrite( PIN_MOTOR_INB, LOW );
        break;

    case Backward:
        digitalWrite( PIN_MOTOR_INA, LOW );
        digitalWrite( PIN_MOTOR_INB, HIGH );
        break;

    case Brake:
        digitalWrite( PIN_MOTOR_INA, HIGH );
        digitalWrite( PIN_MOTOR_INB, HIGH );
        break;
    }
}

}
