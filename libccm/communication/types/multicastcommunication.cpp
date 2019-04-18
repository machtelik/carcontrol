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
            ipv6Address(),
            socketAddress(),
            port(port) {

        inet_pton(AF_INET6, address.c_str(), &ipv6Address);
        socketAddress = {AF_INET6, htons(port), 0, ipv6Address, 0};
    }

    MulticastCommunication::~MulticastCommunication() {
        disconnect();
    }

    bool MulticastCommunication::connect() {
        if (socketDesc != -1) {
            std::cerr << "Already connected" << std::endl;
            return false;
        }

        socketDesc = socket(PF_INET6, SOCK_DGRAM, 0);

        if (socketDesc == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return false;
        }

        int addr = 1;
        if (setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &addr, sizeof(addr)) < 0) {
            std::cerr << "Error while setting SO_REUSEADDR" << std::endl;
            return false;
        }

        sockaddr_in6 bindAddress = { AF_INET6, htons(port), 0, in6addr_any, 0 };
        if (bind(socketDesc, (struct sockaddr *) &bindAddress, sizeof(bindAddress)) < 0) {
            std::cerr << "Could not bind socket" << std::endl;
            return false;
        }

        int loop = 1;
        if (setsockopt(socketDesc, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
            std::cerr << "Error while setting IPV6_MULTICAST_LOOP" << std::endl;
            return false;
        }

        ipv6_mreq group = { ipv6Address, 0 };
        if (setsockopt(socketDesc, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
            std::cerr << "Could not join group" << std::endl;
            return false;
        }

        return true;
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
        auto messageSize = sendto(socketDesc, message->message().data(), message->messageSize(), 0, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
        return messageSize != -1;
    }

    bool MulticastCommunication::receiveMessage(ccm::Message *message) {
        auto messageSize = recvfrom(socketDesc, message->message().data(), message->maxMessageSize(), 0, nullptr, nullptr);
        return messageSize != -1;
    }

} // ccm
