#include "multicast.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "config.h"

namespace ccm {  
  
Multicast::Multicast():
  mSocketDesc(-1)
{

}

Multicast::~Multicast()
{
  disconnect();
}

bool Multicast::connect(const std::string& address, int port)
{
  if(mSocketDesc != -1) {
    std::cerr << "Already connected"  << std::endl;
    return false;
  }

  mSocketDesc = createSocket();
  
  if( mSocketDesc == -1) {
      std::cerr << "Could not create socket"  << std::endl;
    return false;
  }
  
  in6_addr addr = getIPV6Adress(address);
  
  mMulticastSocketAddr = getSocketAdress(port, addr);
  
  if(!setupClientSocket( mSocketDesc, port, addr)) {
    return false;
  }
  
  return true;
}

bool Multicast::disconnect()
{
  if(mSocketDesc == -1) {
    return false;
  }
  
  if(!close(mSocketDesc)) {
    return false;
  }
  
  mSocketDesc = -1;
  
  return true;
}

bool Multicast::send(const char* data, size_t length)
{
  if(length > MAX_PACKET_SIZE) {
    std::cerr << "Message size to large"  << std::endl;
  }
  
  if(sendto( mSocketDesc, data, length, 0, (struct sockaddr *) &mMulticastSocketAddr, sizeof (mMulticastSocketAddr)) == -1) {
    std::cerr << "Could not send data"  << std::endl;
    return false;
  }
    
  return true;
}

size_t Multicast::receive(char* data, size_t maxLength)
{
  sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  
  size_t messageSize = recvfrom( mSocketDesc, data, maxLength, 0, (struct sockaddr *) &addr, &addr_len);
  if (messageSize == -1) {
    std::cerr << "Could not receive data"  << std::endl;
  }
    
  return messageSize;
}

int Multicast::createSocket()
{
  int socketDesc = socket(PF_INET6, SOCK_DGRAM, 0);
  if(socketDesc == -1) {
    std::cerr << "Could not create socket"  << std::endl;
    return -1; 
  }
  return socketDesc;
}

bool Multicast::setupClientSocket(int socketDesc, int port, in6_addr address)
{     
  int addr = 1;
  if(setsockopt (socketDesc, SOL_SOCKET, SO_REUSEADDR, &addr, sizeof (addr)) < 0) {
    std::cerr << "Error while setting SO_REUSEADDR"  << std::endl;
    return false; 
  }
  
  sockaddr_in6 sin = getSocketAdress(port);
  if(bind(socketDesc, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    std::cerr << "Could not bind socket"  << std::endl;
    return false; 
  }
  
  int loop = 1;
  if(setsockopt (socketDesc, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof (loop)) < 0) {
    std::cerr << "Error while setting IPV6_MULTICAST_LOOP"  << std::endl;
    return false; 
  }
  
  ipv6_mreq group;
  group.ipv6mr_multiaddr = address;
  group.ipv6mr_interface = 0;
  
  if(setsockopt (socketDesc, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof (group)) < 0) {
    std::cerr << "Could not join group"  << std::endl;
    return false; 
  }
  
  return true;
}

sockaddr_in6 Multicast::getSocketAdress(int port, in6_addr address)
{
  sockaddr_in6 sin;
  memset (&sin, 0, sizeof (sin));
  sin.sin6_family = AF_INET6;
  sin.sin6_addr = address;
  sin.sin6_port = htons (port);

  return sin;
}

in6_addr Multicast::getIPV6Adress(const std::string& address)
{
  in6_addr ipv6Addr;
  int s = inet_pton (AF_INET6, address.c_str(), &ipv6Addr);
  if (s <= 0) {
    if (s == 0) {
      std::fprintf(stderr, "Not in presentation format: %s\n", address.c_str());
    } else {
      std::cerr << "inet_pton"  << std::endl;
    }
    exit(EXIT_FAILURE);
  }

  return ipv6Addr;
}

  
} // ccm 