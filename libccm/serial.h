#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <netinet/in.h>
#include <string>

#include "datahandler.h"

namespace ccm {

class Serial : public DataHandler {
  
public:
  Serial();
  virtual ~Serial();
  
    bool connect(const std::string& device);
    bool disconnect();
    
    bool send(const char* data, uint16_t length);
    u_int16_t receive(char* data, uint16_t maxLength);
    
    uint8_t deliveryType();

protected:
  
private:
  
};

} // ccm

#endif /* __SERIAL_H__ */
 
 
