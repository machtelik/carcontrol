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
#include "../message/message.h"


namespace ccm {

    MulticastCommunication::MulticastCommunication(const std::string &address, uint16_t port) :
            socketDesc(-1),
            multicastAddress(getIPV6Adress(address)),
            multicastPort(port) {
    }

    MulticastCommunication::~MulticastCommunication() {
        disconnect();
    }

    bool MulticastCommunication::connect() {
        if (socketDesc != -1) {
            std::cerr << "Already connected" << std::endl;
            return false;
        }

        socketDesc = createSocket();

        if (socketDesc == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return false;
        }

        multicastSocketAddress = getSocketAdress(multicastPort, multicastAddress);

        return setupSocket(socketDesc, multicastPort, multicastAddress);
    }

    bool MulticastCommunication::disconnect() {
        if (socketDesc == -1) {
            return false;
        }

        if (!close(socketDesc)) {
            return false;
        }

        socketDesc = -1;

        return true;
    }

    bool MulticastCommunication::sendMessage(const Message *message) {

        if (sendto(socketDesc, message->message().data(), message->messageSize(), 0,
                   (struct sockaddr *) &multicastSocketAddress, sizeof(multicastSocketAddress)) == -1) {
            std::cerr << "Could not send data" << std::endl;
            return false;
        }

        return true;
    }

    void MulticastCommunication::receiveMessages() {
        while (isConnected()) {
            auto message = new Message();

            sockaddr_in addr;
            socklen_t addr_len = sizeof(addr);

            auto messageSize = recvfrom(socketDesc, message->message().data(), message->maxMessageSize(), 0,
                                        (struct sockaddr *) &addr, &addr_len);
            if (messageSize != -1) {
                messageReceived(message);
            }
        }
    }

    int MulticastCommunication::createSocket() {
        return socket(PF_INET6, SOCK_DGRAM, 0);
    }

    bool MulticastCommunication::setupSocket(int socketDesc, uint16_t port, in6_addr address) {
        int addr = 1;
        if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &addr, sizeof(addr)) < 0) {
            std::cerr << "Error while setting SO_REUSEADDR" << std::endl;
            return false;
        }

        sockaddr_in6 sin = getSocketAdress(port);
        if (bind(socketDesc, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
            std::cerr << "Could not bind socket" << std::endl;
            return false;
        }

        int loop = 1;
        if (setsockopt(socketDesc, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
            std::cerr << "Error while setting IPV6_MULTICAST_LOOP" << std::endl;
            return false;
        }

        ipv6_mreq group;
        group.ipv6mr_multiaddr = address;
        group.ipv6mr_interface = 0;

        if (setsockopt(socketDesc, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
            std::cerr << "Could not join group" << std::endl;
            return false;
        }

        return true;
    }

    sockaddr_in6 MulticastCommunication::getSocketAdress(uint16_t port, in6_addr address) {
        sockaddr_in6 sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin6_family = AF_INET6;
        sin.sin6_addr = address;
        sin.sin6_port = htons(port);

        return sin;
    }

    in6_addr MulticastCommunication::getIPV6Adress(const std::string &address) {
        in6_addr ipv6Addr;
        int s = inet_pton(AF_INET6, address.c_str(), &ipv6Addr);
        if (s <= 0) {
            if (s == 0) {
                std::fprintf(stderr, "Not in presentation format: %s\n", address.c_str());
            } else {
                std::cerr << "inet_pton" << std::endl;
            }
            exit(EXIT_FAILURE);
        }

        return ipv6Addr;
    }


} // ccm
