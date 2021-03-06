#ifndef __CCM_SERIALCOMMUNICATION_H__
#define __CCM_SERIALCOMMUNICATION_H__

#include <netinet/in.h>
#include <string>

#include "communication/communication.h"

namespace ccm {

    class SerialCommunication : public Communication {

    public:
        SerialCommunication(std::string device, int baudRate);
        ~SerialCommunication() override;

    protected:

        enum ReceiveState {
            WAIT_FOR_START = 0, READING_CHAR = 1, READING_ESCAPE = 2
        };

        bool connect() override;
        bool disconnect() override;

        bool sendMessage(const Message *message) override;
        bool receiveMessage(Message *message) override;

        char readChar();
        void writeChar(char data);

    private:
        int ttySocket;
        std::string deviceName;
        int baudRate;

    };

} // ccm

#endif /* __CCM_SERIALCOMMUNICATION_H__ */


