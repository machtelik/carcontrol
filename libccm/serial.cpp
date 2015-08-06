#include "serial.h"

#include "config.h"

namespace ccm {
    
Serial::Serial()
{

}

Serial::~Serial()
{

}

    
bool Serial::connect ( const std::string& device)
{

}

bool Serial::disconnect()
{

}

uint8_t Serial::deliveryType() 
{
    return DELIVERY_SERIAL;
}

bool Serial::send ( const char* data, uint16_t length ) 
{

}

u_int16_t Serial::receive ( char* data, uint16_t maxLength )
{

}

}