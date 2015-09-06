#include "message.h"

#include "config.h"

namespace Message
{
    
static const uint8_t HEADER_LENGTH_POS = 2;
static const uint8_t HEADER_SOURCE_POS = 1;
static const uint8_t HEADER_TYPE_POS = 0;
 
uint8_t getType(char *message)
{
    return message[HEADER_TYPE_POS];
}

void setType( uint8_t type, char *message )
{
   message[HEADER_TYPE_POS] = type; 
}

uint8_t getSourceId(char *message)
{
    return message[HEADER_SOURCE_POS];
}

void setSourceId( uint8_t source, char *message )
{
    message[HEADER_SOURCE_POS] = source; 
}

char *getPayload(char *message)
{
    return &message[MESSAGE_HEADER_SIZE];
}

void setPayloadSize( uint16_t size, char *message )
{
    message[HEADER_LENGTH_POS] = size & 0xff;
    message[HEADER_LENGTH_POS + 1] = ( size >> 8 );
}

uint16_t getPayloadSize(char *message)
{
    return ( message[HEADER_LENGTH_POS + 1] << 8 ) + message[HEADER_LENGTH_POS];
}

uint16_t getMessageSize(char *message)
{
    return getPayloadSize(message) + MESSAGE_HEADER_SIZE;
}    

}