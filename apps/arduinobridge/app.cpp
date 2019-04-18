#include "app.h"

#include <communication/message/message.h>

App::App(int argc, char **argv) :
        Component(argc, argv),
        serialCommunication(std::make_unique<ccm::SerialCommunication>("/dev/ttyACM0", 115200)) {
}

bool App::begin() {
    return serialCommunication->start([this](ccm::Message *message) {
        post([this, message] {
            if (!onMessageReceived(message)) {
                delete message;
            }
        });
    });
}

void App::loop() {

}

bool App::onMessageReceived(ccm::Message *message) {
    switch (message->type()) {
        case 0 :
            serialCommunication->send(message);
            return true;
        default:
            break;
    }

    return false;
}

