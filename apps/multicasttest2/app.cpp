#include "app.h"

#include <communication/message/message.h>

#include <iostream>

App::App(int argc, char **argv) :
        Component(argc, argv) {
}

bool App::begin() {
    return true;
}

void App::loop() {

    std::string str("World!");
    auto message = new ccm::Message(1, str.size() + 1);
    std::size_t length = str.copy(message->payload(), str.size());
    message->payload()[length] = '\0';

    sendMessage(message);
}

bool App::onMessageReceived(ccm::Message *message) {
    std::cout << "Got: " << message->payload() << std::endl;
    return false;
}
