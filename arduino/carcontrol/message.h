#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>

namespace Message
{

uint8_t getType(char *message);
void setType( uint8_t type, char *message );

uint8_t getSourceId(char *message);
void setSourceId( uint8_t source, char *message );

char *getPayload(char *message);

void setPayloadSize( uint16_t size, char *message );
uint16_t getPayloadSize(char *message);

uint16_t getMessageSize(char *message);

};

#endif /* __COMMUNICATION_H__ */


