#ifndef CCM_MESSAGE_H
#define CCM_MESSAGE_H

#include <stdint.h>
#include <vector>

namespace ccm {
    class Message {

    public:
        explicit Message();
        explicit Message(uint8_t type, uint8_t payloadSize);
        virtual ~Message() = default;

        uint8_t type() const;
        void setType(uint8_t type);

        uint8_t payloadSize() const;
        void setPayloadSize(uint8_t size);

        const char *payload() const;
        char *payload();

        uint8_t headerSize() const;

        uint8_t messageSize() const;
        uint8_t maxMessageSize() const;

        const std::vector<char> &message() const;
        std::vector<char> &message();

    private:
        static const uint8_t HEADER_SIZE = 2;
        static const uint8_t HEADER_MESSAGE_TYPE_POS = 0;
        static const uint8_t HEADER_MESSAGE_PAYLOAD_SIZE_POS = 1;

        std::vector<char> data;

    };

}

#endif //CCM_MESSAGE_H
