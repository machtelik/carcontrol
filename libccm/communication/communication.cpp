#include <utility>

#include "communication.h"
#include <iostream>

#include "eventloop/eventloop.h"
#include "message/message.h"

namespace ccm {

    Communication::Communication() :
            eventLoop(std::make_unique<EventLoop>(new EventLoop())) {

    }

    bool Communication::start(std::function<void(Message *)> callback) {
        if (connected) {
            std::cerr << "Communication is already connected" << std::endl;
            return false;
        }

        if (!connect()) {
            return false;
        }

        messageCallback = std::move(callback);

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

    void Communication::receiveMessages() {
        while (connected) {
            auto message = new Message();

            if (receiveMessage(message)) {
                eventLoop->post([this, message] {
                    if (messageCallback) {
                        messageCallback.operator()(message);
                    }
                });
            } else {
                delete message;
            }
        }
    }

}
