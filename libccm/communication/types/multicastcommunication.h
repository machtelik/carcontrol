#ifndef __CCM_MULTICASTCOMMUNICATION_H__
#define __CCM_MULTICASTCOMMUNICATION_H__

#include <netinet/in.h>
#include <string>

#include "communication/communication.h"

namespace ccm {

    class MulticastCommunication : public Communication {

    public:
        MulticastCommunication(const std::string &address, uint16_t port);
        ~MulticastCommunication() override;

    protected:

        bool connect() override;
        bool disconnect() override;

        bool sendMessage(const Message *message) override;
        bool receiveMessage(Message *message) override;

    private:
        int socketDesc = -1;

        in6_addr ipv6Address;
        sockaddr_in6 socketAddress;
        uint16_t port;

    };

} // ccm

#endif /* __CCM_MULTICASTCOMMUNICATION_H__ */


