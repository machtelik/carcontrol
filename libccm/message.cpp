#include "message.h"

#include "config.h"

namespace ccm {

Message::Message()  :
    mData(new char[MAX_PACKET_SIZE])
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

void Message::setDataSize ( size_t size ) 
{
    mSize = size;
}

size_t Message::getDataSize()  const
{
    return mSize;
}

size_t Message::getMaxDataSize() const 
{
    return MAX_PACKET_SIZE;
}

}