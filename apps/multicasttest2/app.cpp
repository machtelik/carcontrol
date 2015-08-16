#include "app.h"

#include <communication/message/message.h>

#include <iostream>
#include "communication/message/messagemanager.h"
#include "communication/communicationhandler.h"
#include "communication/types/multicastcommunication.h"

App::App ( int argc, char** argv ) :
        Component ( 0, argc, argv )
{
}

bool App::begin()
{
        return true;
}

bool App::loop()
{

        std::cout << "Sending" << std::endl;

        ccm:: Message *message =  communication()->messages()->getMessage();
        std::string str ( "Message " + getId() );
        std::size_t length = str.copy ( message->getPayload(), str.size() );
        message->getPayload() [length] = '\0';
        message->setPayloadSize ( length + 1 );
         communication()->sendMessage ( ccm::MulticastCommunication::TYPE, message );

        return true;
}

bool App::messageReceived ( const ccm::Message* message )
{
        std::cout << "Got: " << message->getPayload() << std::endl;
        return true;
}
