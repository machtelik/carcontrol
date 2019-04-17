#include "message.h"

#include "../config.h"

Message::Message(char *messageData, uint8_t dataSize) :
        data(messageData),
        dataSize(dataSize) {

}

Message::~Message() {
    delete data;
}

uint8_t Message::type() const {
    return data[HEADER_MESSAGE_TYPE_POS];
}

uint8_t Message::payloadSize() const {
    return data[HEADER_MESSAGE_PAYLOAD_SIZE_POS];
}

const char *Message::payload() const {
    return &data[HEADER_SIZE];
}

uint8_t Message::headerSize() const {
    return HEADER_SIZE;
}

uint8_t Message::messageSize() const {
    return payloadSize() + HEADER_SIZE;
}

uint8_t Message::maxMessageSize() const {
    return dataSize;
}

const char *Message::message() const {
    return data;
}

char *Message::message() {
    return data;
}
