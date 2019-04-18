#ifndef __CCM_SERIALCOMMUNICATION_H__
#define __CCM_SERIALCOMMUNICATION_H__

#include <netinet/in.h>
#include <string>

#include "communication/communication.h"

namespace ccm {

    class SerialCommunication : public Communication {

    public:
        SerialCommunication(const std::string &device, int baudRate);
        ~SerialCommunication() override;

    protected:

        enum ReceiveState {
            WAIT_FOR_START = 0, READING_CHAR = 1, READING_ESCAPE = 2
        };

        bool connect() override;
        bool disconnect() override;

        bool sendMessage(const Message *message) override;
        void receiveMessages() override;

        Message *readMessage();

        static int createSocket(const std::string &device);
        static bool setupSocket(int socketDesc, int speed, int parity);

        char readChar();
        void writeChar(char data);

    private:
        int socketDesc;
        std::string deviceName;
        int baudRate;

    };

} // ccm

#endif /* __CCM_SERIALCOMMUNICATION_H__ */


