#include "app.h"
#include <libccm/message.h>

#include <iostream>
#include "libccm/serialcommunication.h"

static const char* SERIAL_PORT = "/dev/ttyACM0";

App::App ( int argc, char** argv ) :
        Component ( 2, argc, argv )
{
}

bool App::begin()
{
        bool ok = true;
        ok |= addCommunicationMethod(new ccm::SerialCommunication(SERIAL_PORT));
        
        return ok;
}

bool App::loop()
{
        std::cout << "Sending" << std::endl;

        ccm:: Message *message = getMessage();
        message->setCommunicationId(ccm::SerialCommunication::SERIAL_COMMUNICATION);
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
