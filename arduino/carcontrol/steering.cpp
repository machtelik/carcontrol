#include "steering.h"

#include <Arduino.h>
#include "config.h"

Servo Steering::mSteeringServo;


void Steering::init()
{
    mSteeringServo.attach( PIN_STEERING_SERVO );
    steer( 0 );
}

void Steering::steer( int8_t direction )
{
    direction = min( direction, STEERING_DEVIATION );
    direction = max( direction, -STEERING_DEVIATION );

    int16_t pos = STEERING_SERVO_NEUTRAL + ( STEERING_SERVO_MAX_DEVIATION / STEERING_DEVIATION ) * direction;

    mSteeringServo.write( pos );
}

