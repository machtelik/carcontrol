#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <time.h>

#include "config.h"
#include "communication/message/message.h"
#include "communication/types/multicastcommunication.h"
#include "util/periodictimer.h"

namespace ccm {

    Component::Component(int argc, char **argv) :
            EventLoop(),
            periodicTimer(std::make_unique<PeriodicTimer>()),
            communication(std::make_unique<MulticastCommunication>(MULTICAST_IP, MULTICAST_PORT)),
            loopInterval(DEFAULT_LOOP_DURATION) {
    }

    Component::~Component() {
        exit();
    }

    int Component::execute() {
        if (isRunning()) {
            std::cerr << "Component already running" << std::endl;
            return EXIT_FAILURE;
        }

        if (!begin()) {
            std::cerr << "Error while initializing component" << std::endl;
            return EXIT_FAILURE;
        }

        communication->start([this](Message *message) {
            post([this, message] {
                onMessageReceived(message);
                delete message;
            });
        });

        periodicTimer->start(loopInterval, [this] {
            post([this] {
                loop();
            });
        });

        return EventLoop::execute();
    }

    void Component::exit() {
        periodicTimer->stop();
        communication->stop();

        EventLoop::exit();
    }

    void Component::sendMessage(Message *message) {
        communication->send(message);
    }

}


