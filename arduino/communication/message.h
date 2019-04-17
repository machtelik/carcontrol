#ifndef ARDUINO_MESSAGE_H
#define ARDUINO_MESSAGE_H


#include <stdint.h>

class Message {

public:
    explicit Message(char *data, uint8_t dataSize);
    virtual ~Message();

    uint8_t type() const;

    uint8_t payloadSize() const;
    const char *payload() const;

    uint8_t headerSize() const;

    uint8_t messageSize() const;
    uint8_t maxMessageSize() const;

    const char *message() const;
    char *message();

private:
    static const uint8_t HEADER_SIZE = 2;
    static const uint8_t HEADER_MESSAGE_TYPE_POS = 0;
    static const uint8_t HEADER_MESSAGE_PAYLOAD_SIZE_POS = 1;

    uint8_t dataSize;
    char *data;
};

#endif //ARDUINO_MESSAGE_H
