#include "serialcommunication.h"

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "../message/message.h"
#include "config.h"

namespace ccm {

    static const uint8_t MESSAGE_START = 0;
    static const uint8_t MESSAGE_END = 1;
    static const uint8_t MESSAGE_ESCAPE = 2;
    static const uint8_t MESSAGE_START_ESCAPED = 3;
    static const uint8_t MESSAGE_END_ESCAPED = 4;
    static const uint8_t MESSAGE_ESCAPE_ESCAPED = 5;

    SerialCommunication::SerialCommunication(const std::string &device, int baudRate) :
            socketDesc(-1),
            deviceName(device),
            baudRate(baudRate) {
    }

    SerialCommunication::~SerialCommunication() {
        disconnect();
    }

    bool SerialCommunication::connect() {
        if (socketDesc != -1) {
            std::cerr << "Already connected" << std::endl;
            return false;
        }

        socketDesc = createSocket(deviceName);

        if (socketDesc == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return false;
        }

        setupSocket(socketDesc, baudRate, 0);
    }

    bool SerialCommunication::disconnect() {
        if (socketDesc == -1) {
            return false;
        }

        if (!close(socketDesc)) {
            return false;
        }

        socketDesc = -1;
    }

    bool SerialCommunication::sendMessage(const Message *message) {
        if (socketDesc == -1) {
            std::cerr << "Not connected" << std::endl;
            return false;
        }

        writeChar(MESSAGE_START);

        auto data = message->message();

        for (int i = 0; i < message->messageSize(); ++i) {
            switch (data[i]) {
                case MESSAGE_START:
                    writeChar(MESSAGE_ESCAPE);
                    writeChar(MESSAGE_START_ESCAPED);
                    break;

                case MESSAGE_END:
                    writeChar(MESSAGE_ESCAPE);
                    writeChar(MESSAGE_END_ESCAPED);
                    break;

                case MESSAGE_ESCAPE:
                    writeChar(MESSAGE_ESCAPE);
                    writeChar(MESSAGE_ESCAPE_ESCAPED);
                    break;

                default:
                    writeChar(data[i]);
                    break;
            }
        }

        writeChar(MESSAGE_END);

        return true;
    }

    void SerialCommunication::receiveMessages() {
        if (socketDesc == -1) {
            std::cerr << "Not connected" << std::endl;
            return;
        }

        while (isConnected()) {
            auto message = readMessage();

            if (message) {
                messageReceived(message);
            }
        }
    }

    Message *SerialCommunication::readMessage() {
        int readDataBytes = 0;

        auto message = new Message();
        auto data = message->message();

        ReceiveState state = WAIT_FOR_START;

        //Try to read a complete message
        while (readDataBytes < message->maxMessageSize()) {
            char readData = readChar();

            switch (readData) {
                case MESSAGE_START:
                    readDataBytes = 0;
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
                    if (state == READING_CHAR) {
                        return message;
                    } else {
                        state = WAIT_FOR_START;
                    }
                    break;

                case MESSAGE_START_ESCAPED:
                    if (state == READING_ESCAPE) {
                        data[readDataBytes++] = MESSAGE_START;
                        state = READING_CHAR;
                    } else {
                        state = WAIT_FOR_START;
                    }
                    break;

                case MESSAGE_ESCAPE_ESCAPED:
                    if (state == READING_ESCAPE) {
                        data[readDataBytes++] = MESSAGE_ESCAPE;
                        state = READING_CHAR;
                    } else {
                        state = WAIT_FOR_START;
                    }
                    break;

                case MESSAGE_END_ESCAPED:
                    if (state == READING_ESCAPE) {
                        data[readDataBytes++] = MESSAGE_END;
                        state = READING_CHAR;
                    } else {
                        state = WAIT_FOR_START;
                    }
                    break;


                default:
                    if (state == READING_CHAR) {
                        data[readDataBytes++] = readData;
                    } else {
                        state = WAIT_FOR_START;
                    }
                    break;
            }
        }

        return nullptr;
    }

    int SerialCommunication::createSocket(const std::string &device) {
        int socketDesc = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (socketDesc == -1) {
            return -1;
        }
        return socketDesc;
    }

    bool SerialCommunication::setupSocket(int socketDesc, int speed, int parity) {
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr(socketDesc, &tty) != 0) {
            std::cerr << "error from tcgetattr" << std::endl;
            return false;
        }

        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_iflag &= ~IGNBRK;
        tty.c_lflag = 0;
        tty.c_oflag = 0;
        tty.c_cc[VMIN] = 1;

        tty.c_iflag &= ~(IXON | IXOFF | IXANY);

        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~(PARENB | PARODD);
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr(socketDesc, TCSANOW, &tty) != 0) {
            std::cerr << "error from tcsetattr" << std::endl;
            return false;
        }

        return true;
    }

    char SerialCommunication::readChar() {
        char data;
        read(socketDesc, &data, 1);
        return data;
    }

    void SerialCommunication::writeChar(char data) {
        write(socketDesc, &data, 1);
    }

}