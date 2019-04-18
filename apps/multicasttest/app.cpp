#include "app.h"

#include <iostream>
#include <chrono>

#include <communication/message/message.h>

App::App( int argc, char **argv ) :
    Component( argc, argv )
{
}

bool App::begin()
{
    return true;
}

void App::loop()
{
    static auto time = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::time_point_cast<std::chrono::nanoseconds>( t ).time_since_epoch().count() << "ns\n";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>( t - time ).count() << "ms\n";

    time = t;

    std::string str("Hello ");
    auto message = new ccm::Message(0, str.size() + 1);
    std::size_t length = str.copy(message->payload(), str.size());
    message->payload()[length] = '\0';

    sendMessage(message);
}

bool App::onMessageReceived(ccm::Message *message) {
    std::cout << "Got: " << message->payload() << std::endl;
    return false;
}

