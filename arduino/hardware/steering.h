#ifndef _STEERING_H_
#define _STEERING_H_

#include <stdint.h>

class Servo;

class Steering {

public:
    explicit Steering(uint8_t pinServo);
    ~Steering();

    void neutral();

    int8_t position() const;
    void setPosition(int8_t position);


private:
    static const int8_t NEUTRAL_OFFSET = 90;
    static const int8_t MAX_STEER_OFFSET = 45;

    Servo *steeringServo;

};

#endif // _STEERING_H_

