#ifndef __MULTICAST_H__
#define __MULTICAST_H__

#include <netinet/in.h>

#include <string>

namespace ccm {

class Multicast {
  
public:
  Multicast();
  virtual ~Multicast();
  
    bool connect(const std::string &address, int port);
    bool disconnect();
    
    bool send(const char* data, size_t length);
    size_t receive(char *data, size_t maxLength);
  
    static sockaddr_in6 getSocketAdress(int port, in6_addr address = in6addr_any);
    static in6_addr getIPV6Adress(const std::string& address);
protected:
  
  static int createSocket();
  static bool setupClientSocket(int socketDesc, int port, in6_addr address);
  
private:
  int mSocketDesc;
  
  sockaddr_in6 mMulticastSocketAddr;
  
};

} // ccm

#endif /* __MULTICAST_H__ */
 
 
