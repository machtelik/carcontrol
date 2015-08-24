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

bool App::messageReceived( uint8_t communicationType, const ccm::Message *message )
{
    
    if( communicationType == ccm::MulticastCommunication::TYPE) {
        ccm::Message *messageCopy = communication()->messages()->getMessageCopy( message );
        communication()->sendMessage( ccm::SerialCommunication::TYPE, messageCopy );
    } else if( communicationType == ccm::SerialCommunication::TYPE) {
        ccm::Message *messageCopy = communication()->messages()->getMessageCopy( message );
        communication()->sendMessage( ccm::MulticastCommunication::TYPE, messageCopy );
    }
    return true;
}
