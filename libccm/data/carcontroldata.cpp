#include "carcontroldata.h"

#include "communication/message/message.h"

namespace ccm
{

static const uint8_t PAYLOAD_SIZE = 3;

CarControlData::CarControlData() :
    mSpeed( 0 ),
    mSteering( 0 ),
    mMotorStatus( Neutral )
{
}

CarControlData::~CarControlData()
{

}

void CarControlData::setSpeed( uint8_t speed )
{
    mSpeed = speed;
}

uint8_t CarControlData::getSpeed() const
{
    return mSpeed;
}

void CarControlData::setSteering( uint8_t steering )
{
    mSteering = steering;
}

uint8_t CarControlData::getSteering() const
{
    return mSteering;
}

void CarControlData::setMotorStatus( CarControlData::MotorStatus motorStatus )
{
    mMotorStatus = motorStatus;
}

CarControlData::MotorStatus CarControlData::getMotorStatus() const
{
    return mMotorStatus;
}

bool CarControlData::fromMessage( const Message *message )
{
    if( message->getType() != TYPE || message->getMessageSize() != PAYLOAD_SIZE ) {
        return false;
    }

    mSpeed = message->getPayload() [0];
    mMotorStatus = ( MotorStatus ) message->getPayload() [1];
    mSteering = message->getPayload() [2];

    return true;
}

bool CarControlData::toMessage( Message *message ) const
{
    message->getPayload() [0] = mSpeed;
    message->getPayload() [1] = mMotorStatus;
    message->getPayload() [2] = mSteering;
    message->setPayloadSize( PAYLOAD_SIZE );

    return true;
}

}
