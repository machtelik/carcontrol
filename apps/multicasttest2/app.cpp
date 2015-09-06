#include "app.h"

#include <communication/message/message.h>

#include <iostream>
#include "communication/message/messagemanager.h"
#include "communication/communicationhandler.h"

App::App( int argc, char **argv ) :
    Component( 0, argc, argv )
{
}

bool App::begin()
{
    return true;
}

bool App::loop()
{

    ccm:: Message *message =  communication()->messages()->getMessage();
    std::string str( "Message" );
    std::size_t length = str.copy( message->getPayload(), str.size() );
    message->getPayload() [length] = '\0';
    message->setPayloadSize( length + 1 );
    sendMessage( message );

    return true;
}

bool App::messageReceived( uint8_t communicationType, const ccm::Message *message )
{
    std::cout << "Got: " << message->getPayload() << std::endl;
    return true;
}
