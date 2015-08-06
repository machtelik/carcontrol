#ifndef __MULTICAST_H__
#define __MULTICAST_H__

#include <netinet/in.h>
#include <string>

#include "datahandler.h"

namespace ccm {

class Multicast : public DataHandler {
  
public:
  Multicast();
  virtual ~Multicast();
  
    bool connect(const std::string& address, uint16_t port);
    bool disconnect();
    
    bool send(const char* data, uint16_t length) const;
    uint16_t receive(char* data, uint16_t maxLength) const;
    
    uint8_t deliveryType();
  
    static sockaddr_in6 getSocketAdress(uint16_t port, in6_addr address = in6addr_any);
    static in6_addr getIPV6Adress(const std::string& address);
protected:
  
  static int createSocket();
  static bool setupSocket(int socketDesc, uint16_t port, in6_addr address);
  
private:
  int mSocketDesc;
  
  sockaddr_in6 mMulticastSocketAddr;
  
};

} // ccm

#endif /* __MULTICAST_H__ */
 
 
