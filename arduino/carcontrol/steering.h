#ifndef _STEERING_H_
#define _STEERING_H_

#include <stdint.h>
#include <Servo.h>

class Steering
{

public:

    static void init();

    static void neutral();
    static void steer( int8_t direction );
    static int8_t position();

private:

    static Servo mSteeringServo;

};

#endif // _STEERING_H_

