#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>

class Message;

class Communication {

public:
    class MessageCallback {
    public:
        virtual void onNewMessage(const Message *message) = 0;
    };

    explicit Communication(uint32_t serialSpeed);
    virtual ~Communication();

    void send(const Message *message);
    void receive();

    void setMessageCallback(MessageCallback *messageCallback);

private:
    static const uint8_t MESSAGE_START = 0;
    static const uint8_t MESSAGE_END = 1;
    static const uint8_t MESSAGE_ESCAPE = 2;
    static const uint8_t MESSAGE_START_ESCAPED = 3;
    static const uint8_t MESSAGE_END_ESCAPED = 4;
    static const uint8_t MESSAGE_ESCAPE_ESCAPED = 5;

    MessageCallback *messageCallback = nullptr;

    enum ReceiveState {
        WAIT_FOR_START = 0, READING_CHAR = 1, READING_ESCAPE = 2
    } state = WAIT_FOR_START;


    uint16_t readMessageBytes = 0;
    Message *bufferMessage;

};

#endif /* __COMMUNICATION_H__ */