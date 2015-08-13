#include "app.h"
#include <libccm/data/message.h>

#include <iostream>

#include "libccm/data/messagemanager.h"

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
        std::cout << "Sending" << std::endl;

        ccm:: Message *message = messageManager()->getMessage();
        std::string str ( "Message " + getId() );
        std::size_t length = str.copy ( message->getPayload(), str.size() );
        message->getPayload() [length] = '\0';
        message->setPayloadSize ( length + 1 );
        sendMessage ( message );
        return true;
}

bool App::messageReceived ( const ccm::Message* message )
{
        std::cout << "Got: " << message->getPayload() << std::endl;
        return true;
}
