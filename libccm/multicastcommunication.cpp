#include "multicastcommunication.h"

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

namespace ccm
{

MulticastCommunication::MulticastCommunication(const std::string& address, uint16_t port) :
        Communication(MULTICAST_COMMUNICATION),
        mSocketDesc ( -1 ),
        mAddress(getIPV6Adress ( address )),
        mPort(port)
{
}

MulticastCommunication::~MulticastCommunication()
{
        disconnect();
}

bool MulticastCommunication::connect (  )
{
        if ( mSocketDesc != -1 ) {
                std::cerr << "Already connected"  << std::endl;
                return false;
        }
        
        mSocketDesc = createSocket();

        if ( mSocketDesc == -1 ) {
                std::cerr << "Could not create socket"  << std::endl;
                return false;
        }
        
       mMulticastAddr = getSocketAdress ( mPort, mAddress );

        if ( !setupSocket ( mSocketDesc, mPort, mAddress ) ) {
                return false;
        }

        return true;
}

bool MulticastCommunication::disconnect()
{
        if ( mSocketDesc == -1 ) {
                return false;
        }

        if ( !close ( mSocketDesc ) ) {
                return false;
        }

        mSocketDesc = -1;

        return true;
}

bool MulticastCommunication::send ( const char* data, uint16_t length )
{
        if ( length > MESSAGE_MAX_SIZE ) {
                std::cerr << "Message size to large"  << std::endl;
        }

        if ( sendto ( mSocketDesc, data, length, 0, ( struct sockaddr * ) &mMulticastAddr, sizeof ( mMulticastAddr ) ) == -1 ) {
                std::cerr << "Could not send data"  << std::endl;
                return false;
        }

        return true;
}

uint16_t MulticastCommunication::receive ( char* data, uint16_t maxLength )
{
        sockaddr_in addr;
        socklen_t addr_len = sizeof ( addr );

        uint16_t messageSize = recvfrom ( mSocketDesc, data, maxLength, 0, ( struct sockaddr * ) &addr, &addr_len );

        return messageSize;
}

int MulticastCommunication::createSocket()
{
        return  socket ( PF_INET6, SOCK_DGRAM, 0 );
}

bool MulticastCommunication::setupSocket ( int socketDesc, uint16_t port, in6_addr address )
{
        int addr = 1;
        if ( setsockopt ( socketDesc, SOL_SOCKET, SO_REUSEADDR, &addr, sizeof ( addr ) ) < 0 ) {
                std::cerr << "Error while setting SO_REUSEADDR"  << std::endl;
                return false;
        }

        sockaddr_in6 sin = getSocketAdress ( port );
        if ( bind ( socketDesc, ( struct sockaddr * ) &sin, sizeof ( sin ) ) < 0 ) {
                std::cerr << "Could not bind socket"  << std::endl;
                return false;
        }

        int loop = 1;
        if ( setsockopt ( socketDesc, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof ( loop ) ) < 0 ) {
                std::cerr << "Error while setting IPV6_MULTICAST_LOOP"  << std::endl;
                return false;
        }

        ipv6_mreq group;
        group.ipv6mr_multiaddr = address;
        group.ipv6mr_interface = 0;

        if ( setsockopt ( socketDesc, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof ( group ) ) < 0 ) {
                std::cerr << "Could not join group"  << std::endl;
                return false;
        }

        return true;
}

sockaddr_in6 MulticastCommunication::getSocketAdress ( uint16_t port, in6_addr address )
{
        sockaddr_in6 sin;
        memset ( &sin, 0, sizeof ( sin ) );
        sin.sin6_family = AF_INET6;
        sin.sin6_addr = address;
        sin.sin6_port = htons ( port );

        return sin;
}

in6_addr MulticastCommunication::getIPV6Adress ( const std::string& address )
{
        in6_addr ipv6Addr;
        int s = inet_pton ( AF_INET6, address.c_str(), &ipv6Addr );
        if ( s <= 0 ) {
                if ( s == 0 ) {
                        std::fprintf ( stderr, "Not in presentation format: %s\n", address.c_str() );
                } else {
                        std::cerr << "inet_pton"  << std::endl;
                }
                exit ( EXIT_FAILURE );
        }

        return ipv6Addr;
}


} // ccm
