#ifndef _STEERING_H_
#define _STEERING_H_

#include <stdint.h>

namespace Steering
{

    void setup();

    void neutral();
    void steer( int8_t direction );
    int8_t position();

};

#endif // _STEERING_H_

