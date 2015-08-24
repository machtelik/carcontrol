#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>

#include "config.h"

class Communication
{

public:

    static void init();
    
    static void send( const char *data, uint16_t length );
    static void receive( void(*dispatch)(char*, uint16_t) );
    
    static char* sendBuffer();

private:
    static int mReadDataBytes;

    static bool mReadingData;
    static bool mReadEscape;
    
    static char mReceiveBuffer[MESSAGE_MAX_SIZE];
    static char mSendBuffer[MESSAGE_MAX_SIZE];

};

#endif /* __COMMUNICATION_H__ */


