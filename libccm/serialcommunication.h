#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <netinet/in.h>
#include <string>

#include "communication.h"

namespace ccm {

class SerialCommunication : public Communication {
  
public:
          static const uint8_t SERIAL_COMMUNICATION = 1;
    
  SerialCommunication(const std::string& device);
  virtual ~SerialCommunication();
  
    bool connect();
    bool disconnect();
    
    bool send(const char* data, uint16_t length);
    u_int16_t receive(char* data, uint16_t maxLength);

protected:
    
    static int createSocket(const std::string& device);
    static bool setupSocket (int socketDesc, int speed, int parity);
  
private:
    int mSocketDesc;
    std::string mDevice;
    
};

} // ccm

#endif /* __SERIAL_H__ */
 
 
