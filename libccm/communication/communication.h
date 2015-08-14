#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>

namespace ccm {

class Communication {
  
public:
    
    Communication(uint8_t communicationType);
    
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;

    virtual bool send(const char* data, uint16_t length) = 0;
    virtual uint16_t receive(char* data, uint16_t maxLength) = 0;
    
    uint8_t communicationType();
    
private:
    
    uint8_t mCommunicationType;
  
};

} // ccm

#endif /* __COMMUNICATION_H__ */
 
 
