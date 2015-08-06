#ifndef __MULTICAST_H__
#define __MULTICAST_H__

#include <netinet/in.h>

#include <string>

namespace ccm {

class Multicast {
  
public:
  Multicast();
  virtual ~Multicast();
  
    bool connect(const std::string& address, uint16_t port);
    bool disconnect();
    
    bool send(const char* data, uint16_t length);
    u_int16_t receive(char* data, uint16_t maxLength);
  
    static sockaddr_in6 getSocketAdress(uint16_t port, in6_addr address = in6addr_any);
    static in6_addr getIPV6Adress(const std::string& address);
protected:
  
  static int createSocket();
  static bool setupClientSocket(int socketDesc, uint16_t port, in6_addr address);
  
private:
  int mSocketDesc;
  
  sockaddr_in6 mMulticastSocketAddr;
  
};

} // ccm

#endif /* __MULTICAST_H__ */
 
 
