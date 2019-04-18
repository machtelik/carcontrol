#include "message.h"
#include <string.h>

#include "config.h"

namespace ccm {

    Message::Message() : data(MESSAGE_MAX_SIZE) {
        setPayloadSize(MESSAGE_MAX_SIZE - HEADER_SIZE);
    }

    Message::Message(uint8_t type, uint8_t payloadSize) : data(HEADER_SIZE + payloadSize) {
        setType(type);
        setPayloadSize(payloadSize);
    }

    uint8_t Message::type() const {
        return data[HEADER_MESSAGE_TYPE_POS];
    }

    void Message::setType(uint8_t type) {
        data[HEADER_MESSAGE_TYPE_POS] = type;
    }

    uint8_t Message::payloadSize() const {
        return data[HEADER_MESSAGE_PAYLOAD_SIZE_POS];
    }

    void Message::setPayloadSize(uint8_t size) {
        data[HEADER_MESSAGE_PAYLOAD_SIZE_POS] = size;
    }

    const char *Message::payload() const {
        return &data[HEADER_SIZE];
    }

    char *Message::payload() {
        return &data[HEADER_SIZE];
    }

    uint8_t Message::headerSize() const {
        return HEADER_SIZE;
    }

    uint8_t Message::messageSize() const {
        return payloadSize() + HEADER_SIZE;
    }

    uint8_t Message::maxMessageSize() const {
        return data.size();
    }

    const std::vector<char> &Message::message() const {
        return data;
    }

    std::vector<char> &Message::message() {
        return data;
    }

}