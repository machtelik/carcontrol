#include "message.h"

#include "config.h"

namespace ccm {

static const uint8_t HEADER_LENGTH_POS = 0;
static const uint8_t HEADER_SOURCE_POS = 2;
static const uint8_t HEADER_TYPE_POS = 3;
    
Message::Message()  :
    mData(new char[MESSAGE_MAX_SIZE]),
    mDeliveryType(DELIVERY_MULTICAST)
{
}

Message::~Message()
{
    delete[] mData;
}

char* Message::getData()
{
    return mData;
}

const char* Message::getData() const 
{
    return mData;
}

char* Message::getPayload()
{
    return &mData[MESSAGE_HEADER_SIZE];
}

const char* Message::getPayload() const 
{
    return &mData[MESSAGE_HEADER_SIZE];
}

void Message::setPayloadSize ( uint16_t size ) 
{
    size = size < getMaxPayloadSize() ? size : getMaxPayloadSize();
    
    size += MESSAGE_HEADER_SIZE;
    
    mData[HEADER_LENGTH_POS] = size & 0xff;
    mData[HEADER_LENGTH_POS+1] = (size >> 8);
}

uint16_t Message::getPayloadSize()  const
{
    return getMessageSize()  - MESSAGE_HEADER_SIZE;
}

uint16_t Message::getMaxPayloadSize() 
{
    return MESSAGE_MAX_SIZE - MESSAGE_HEADER_SIZE;
}

uint16_t Message::getMaxMessageSize() 
{
    return MESSAGE_MAX_SIZE;
}

uint16_t Message::getMessageSize() const
{
    return (mData[HEADER_LENGTH_POS+1] << 8) + mData[HEADER_LENGTH_POS];
}

void Message::setSourceId ( uint8_t source )
{
    mData[HEADER_SOURCE_POS] = source;
}

uint8_t Message::getSourceId() const
{
    return mData[HEADER_SOURCE_POS];
}

void Message::setType ( uint8_t type )
{
    mData[HEADER_TYPE_POS] = type;
}

uint8_t Message::getType() const
{
    return mData[HEADER_TYPE_POS];
}

uint8_t Message::getDeliveryType() const
{
    return mDeliveryType;
}

void Message::setDeliveryType ( uint8_t type ) 
{
    mDeliveryType = type;
}


}