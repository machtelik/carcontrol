#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>

namespace Communication
{

    void setup();
    
    void send( const char *data, uint16_t length );
    void receive( void(*dispatch)(char*, uint16_t) );
    
    char* sendBuffer();

};

#endif /* __COMMUNICATION_H__ */


