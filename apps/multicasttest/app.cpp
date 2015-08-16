#include "app.h"

#include <iostream>
#include <chrono>

#include <communication/message/message.h>
#include "communication/message/messagemanager.h"
#include "communication/communicationhandler.h"
#include "communication/types/multicastcommunication.h"

App::App ( int argc, char** argv ) :
        Component ( 1, argc, argv )
{
}

bool App::begin()
{

        return true;
}

bool App::loop()
{

        
        static auto time = std::chrono::high_resolution_clock::now();
        auto t = std::chrono::high_resolution_clock::now();
    
        std::cout << std::chrono::time_point_cast<std::chrono::nanoseconds>(t).time_since_epoch().count() << "ns\n";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t - time).count() << "ms\n";
    
        time = t;

    
        std::cout << "Sending" << std::endl;

        ccm:: Message *message = communication()->messages()->getMessage();
        std::string str ( "Message " + getId() );
        std::size_t length = str.copy ( message->getPayload(), str.size() );
        message->getPayload() [length] = '\0';
        message->setPayloadSize ( length + 1 );
         communication()->sendMessage ( ccm::MulticastCommunication::TYPE,  message );
        return true;
}

bool App::messageReceived ( const ccm::Message* message )
{
        std::cout << "Got: " << message->getPayload() << std::endl;
        return true;
}
