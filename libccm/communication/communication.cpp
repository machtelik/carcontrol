#include "communication.h"
#include <iostream>

#include "eventloop/eventloop.h"
#include "message/message.h"

namespace ccm {

    Communication::Communication() :
            eventLoop(std::make_unique<EventLoop>(new EventLoop())) {

    }

    bool Communication::start() {
        if (connected) {
            std::cerr << "Communication is already connected" << std::endl;
            return false;
        }

        if (!connect()) {
            return false;
        }

        eventThread = std::make_unique<std::thread>(new std::thread(&EventLoop::execute, eventLoop));
        receiveThread = std::make_unique<std::thread>(new std::thread(&Communication::receiveMessages));

        connected = true;

        return true;
    }

    void Communication::stop() {
        if (!connected) {
            return;
        }

        connected = false;
        eventLoop->exit();

        eventThread->join();
        receiveThread->join();

        eventThread = nullptr;
        receiveThread = nullptr;
    }

    bool Communication::isConnected() {
        return connected;
    }

    void Communication::send(Message *message) {
        eventLoop->post([this, message] {
            sendMessage(message);

            delete message;
        });
    }

    void Communication::setMessageCallback(std::function<void(const Message *)> callback) {
        messageCallback = callback;
    }

    void Communication::messageReceived(Message *message) {
        eventLoop->post([this, message] {
            if (messageCallback) {
                messageCallback.operator()(message);
            }

            delete message;
        });
    }


}
