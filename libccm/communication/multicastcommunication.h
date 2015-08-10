#ifndef __MULTICASTCOMMUNICATION_H__
#define __MULTICASTCOMMUNICATION_H__

#include <netinet/in.h>
#include <string>

#include "communication.h"

namespace ccm {

class MulticastCommunication : public Communication {
  
public:
          static const uint8_t TYPE = 0;
    
  MulticastCommunication(const std::string& address, uint16_t port);
  virtual ~MulticastCommunication();
  
    bool connect();
    bool disconnect();
    
    bool send(const char* data, uint16_t length);
    uint16_t receive(char* data, uint16_t maxLength);
  
    static sockaddr_in6 getSocketAdress(uint16_t port, in6_addr address = in6addr_any);
    static in6_addr getIPV6Adress(const std::string& address);
protected:
  
  static int createSocket();
  static bool setupSocket(int socketDesc, uint16_t port, in6_addr address);
  
private:
  int mSocketDesc;
  
  in6_addr mAddress;
  uint16_t mPort;
  
  sockaddr_in6 mMulticastAddr;
  
};

} // ccm

#endif /* __MULTICASTCOMMUNICATION_H__ */
 
 
