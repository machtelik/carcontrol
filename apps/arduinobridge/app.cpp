#include "app.h"

#include <iostream>

#include <communication/message/messagemanager.h>
#include <communication/message/message.h>
#include "communication/types/serialcommunication.h"
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
    if( message->getType() == ccm::CarControlData::TYPE ) {
        ccm::Message *messageCopy = communication()->messages()->getMessageCopy( message );
        communication()->sendMessage( ccm::SerialCommunication::TYPE, messageCopy );
    }
    return true;
}
