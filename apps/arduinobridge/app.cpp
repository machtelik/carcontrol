#include "app.h"

#include <iostream>

#include <communication/message/messagemanager.h>
#include <communication/message/message.h>
#include "communication/types/serialcommunication.h"
#include "communication/types/multicastcommunication.h"
#include <communication/communicationhandler.h>
#include "data/carcontroldata.h"

static const char *SERIAL_PORT = "/dev/ttyACM0";
static const uint32_t SERIAL_BAUD_RATE = 9600;

App::App( int argc, char **argv ) :
    Component( 2, argc, argv )
{
}

bool App::begin()
{
    bool ok = true;
    ok |= communication()->addCommunicationMethod( new ccm::SerialCommunication( SERIAL_PORT, SERIAL_BAUD_RATE ) );

    return ok;
}

bool App::loop()
{
    return true;
}

bool App::messageReceived( const ccm::Message *message )
{
    if( message->getSourceId() != 42) {
        std::cout << "Network: " << message->getPayload() << std::endl;
        ccm::Message *messageCopy = communication()->messages()->getMessageCopy( message );
        messageCopy->setSourceId(getId());
        communication()->sendMessage( ccm::SerialCommunication::TYPE, messageCopy );
    } else {
        ccm::Message *messageCopy = communication()->messages()->getMessageCopy( message );
        std::cout << "Serial: " << message->getPayload() << std::endl;
        messageCopy->setSourceId(getId());
        communication()->sendMessage( ccm::MulticastCommunication::TYPE, messageCopy );
    }
    return true;
}
