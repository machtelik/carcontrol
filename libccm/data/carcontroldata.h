#ifndef __CARCONTROLDATA_H__
#define __CARCONTROLDATA_H__

#include <stddef.h>
#include <stdint.h>

#include "messageable.h"

namespace ccm
{

class CarControlData : public Messageable
{

public:
    static const uint8_t TYPE = 0;

    enum MotorStatus { Neutral = 0, Forward = 1, Backward = 2, Brake = 3 };

    CarControlData();
    virtual ~CarControlData();

    void setSpeed( uint8_t speed );
    uint8_t getSpeed() const;

    void setSteering( uint8_t steering );
    uint8_t getSteering() const;

    void setMotorStatus( MotorStatus motorStatus );
    MotorStatus getMotorStatus() const;

    virtual bool fromMessage( const Message *message );
    virtual bool toMessage( Message *message ) const;

protected:

private:

    uint8_t mSpeed;
    int8_t mSteering;
    MotorStatus mMotorStatus;

};

} // ccm

#endif /* __CARCONTROLDATA_H__ */

