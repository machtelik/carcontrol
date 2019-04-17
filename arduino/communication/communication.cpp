#include "communication.h"

#include <Arduino.h>
#include "message.h"
#include "../config.h"

Communication::Communication(uint32_t serialSpeed) :
        bufferMessage(new Message(new char[MAX_MESSAGE_SIZE], MAX_MESSAGE_SIZE)) {

    Serial.begin(serialSpeed);

}

Communication::~Communication() {
    delete bufferMessage;
}

void Communication::send(const Message *message) {
    auto data = message->message();

    Serial.write(MESSAGE_START);

    for (int i = 0; i < message->messageSize(); ++i) {
        switch (data[i]) {
            case MESSAGE_START:
                Serial.write(MESSAGE_ESCAPE);
                Serial.write(MESSAGE_START_ESCAPED);
                break;

            case MESSAGE_END:
                Serial.write(MESSAGE_ESCAPE);
                Serial.write(MESSAGE_END_ESCAPED);
                break;

            case MESSAGE_ESCAPE:
                Serial.write(MESSAGE_ESCAPE);
                Serial.write(MESSAGE_ESCAPE_ESCAPED);
                break;

            default:
                Serial.write(data[i]);
                break;
        }
    }

    Serial.write(MESSAGE_END);
}

void Communication::receive() {
    while (Serial.available()) {
        char readData = Serial.read();

        if (readMessageBytes >= bufferMessage->maxMessageSize()) {
            state = WAIT_FOR_START;
        }

        auto receiveBuffer = bufferMessage->message();

        switch (readData) {
            case MESSAGE_START:
                readMessageBytes = 0;
                state = READING_CHAR;
                break;

            case MESSAGE_ESCAPE:
                if (state == READING_CHAR) {
                    state = READING_ESCAPE;
                } else {
                    state = WAIT_FOR_START;
                }
                break;

            case MESSAGE_END:
                if (state == READING_CHAR && messageCallback != nullptr) {
                    messageCallback->onNewMessage(bufferMessage);
                }
                state = WAIT_FOR_START;
                break;

            case MESSAGE_START_ESCAPED:
                if (state == READING_ESCAPE) {
                    receiveBuffer[readMessageBytes++] = MESSAGE_START;
                    state = READING_CHAR;
                } else {
                    state = WAIT_FOR_START;
                }
                break;

            case MESSAGE_ESCAPE_ESCAPED:
                if (state == READING_ESCAPE) {
                    receiveBuffer[readMessageBytes++] = MESSAGE_ESCAPE;
                    state = READING_CHAR;
                } else {
                    state = WAIT_FOR_START;
                }
                break;

            case MESSAGE_END_ESCAPED:
                if (state == READING_ESCAPE) {
                    receiveBuffer[readMessageBytes++] = MESSAGE_END;
                    state = READING_CHAR;
                } else {
                    state = WAIT_FOR_START;
                }
                break;

            default:
                if (state == READING_CHAR) {
                    receiveBuffer[readMessageBytes++] = readData;
                } else {
                    state = WAIT_FOR_START;
                }
                break;
        }
    }
}

void Communication::setMessageCallback(Communication::MessageCallback *messageCallback) {
    this->messageCallback = messageCallback;
}




