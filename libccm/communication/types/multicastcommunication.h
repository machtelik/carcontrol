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
        void receiveMessages() override;

        static int createSocket();
        static bool setupSocket(int socketDesc, uint16_t port, in6_addr address);

        static sockaddr_in6 getSocketAdress(uint16_t port, in6_addr address = in6addr_any);
        static in6_addr getIPV6Adress(const std::string &address);

    private:
        int socketDesc;

        in6_addr multicastAddress;
        uint16_t multicastPort;

        sockaddr_in6 multicastSocketAddress;

    };

} // ccm

#endif /* __CCM_MULTICASTCOMMUNICATION_H__ */


