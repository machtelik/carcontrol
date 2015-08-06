#ifndef __DATAHANDLER_H__
#define __DATAHANDLER_H__

#include <stdint.h>

namespace ccm {

class DataHandler {
  
public:
    
    virtual bool send(const char* data, uint16_t length) const = 0;
    virtual uint16_t receive(char* data, uint16_t maxLength) const = 0;
    
    virtual uint8_t deliveryType() = 0;
  
};

} // ccm

#endif /* __DATAHANDLER_H__ */
 
 
