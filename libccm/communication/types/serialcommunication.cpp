#include <utility>

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

    SerialCommunication::SerialCommunication(std::string device, int baudRate) :
            ttySocket(-1),
            deviceName(std::move(device)),
            baudRate(baudRate) {
    }

    SerialCommunication::~SerialCommunication() {
        disconnect();
    }

    bool SerialCommunication::connect() {
        if (ttySocket != -1) {
            std::cerr << "Already connected" << std::endl;
            return false;
        }

        ttySocket = open(deviceName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (ttySocket == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return false;
        }

        if(!isatty(ttySocket)) {
            std::cerr << "Device is not a tty socket" << std::endl;
            disconnect();
            return false;
        }

        termios ttyConfig;
        if (tcgetattr(ttySocket, &ttyConfig) == -1) {
            std::cerr << "Could not get tty attributes" << std::endl;
            disconnect();
            return false;
        }

        ttyConfig.c_cflag &= ~PARENB;
        ttyConfig.c_cflag &= ~CSTOPB;
        ttyConfig.c_cflag &= ~CSIZE;
        ttyConfig.c_cflag |= CS8;
        ttyConfig.c_cflag &= ~CRTSCTS;
        ttyConfig.c_cflag |= CREAD | CLOCAL;
        ttyConfig.c_iflag &= ~(IXON | IXOFF | IXANY);
        ttyConfig.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        ttyConfig.c_oflag &= ~OPOST;
        ttyConfig.c_cc[VMIN] = 1;
        ttyConfig.c_cc[VTIME] = 0;

        if(cfsetospeed(&ttyConfig, baudRate) != 0 || cfsetispeed(&ttyConfig, baudRate) != 0) {
            std::cerr << "Could not set baud raute" << std::endl;
            disconnect();
            return false;
        }

        if (tcsetattr(ttySocket, TCSANOW, &ttyConfig) != 0) {
            std::cerr << "Could not set tty attributes" << std::endl;
            disconnect();
            return false;
        }

        return true;
    }

    bool SerialCommunication::disconnect() {
        if (ttySocket == -1) {
            return false;
        }

        if (!close(ttySocket)) {
            return false;
        }

        ttySocket = -1;
    }

    bool SerialCommunication::sendMessage(const Message *message) {
        if (ttySocket == -1) {
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

    bool SerialCommunication::receiveMessage(Message *message)  {
        int readDataBytes = 0;

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
                        return true;
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

        return false;
    }

    char SerialCommunication::readChar() {
        char data;
        read(ttySocket, &data, 1);
        return data;
    }

    void SerialCommunication::writeChar(char data) {
        write(ttySocket, &data, 1);
    }

}