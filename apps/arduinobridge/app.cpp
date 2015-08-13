#include "app.h"
#include <libccm/data/message.h>

#include <iostream>
#include "libccm/communication/serialcommunication.h"
#include "libccm/data/carcontroldata.h"
#include <libccm/data/messagemanager.h>

static const char* SERIAL_PORT = "/dev/ttyACM0";
static const uint32_t SERIAL_BAUD_RATE = 9600;

App::App ( int argc, char** argv ) :
        Component ( 2, argc, argv )
{
}

bool App::begin()
{
        bool ok = true;
        ok |= addCommunicationMethod(new ccm::SerialCommunication(SERIAL_PORT, SERIAL_BAUD_RATE));
        
        return ok;
}

bool App::loop()
{
        return true;
}

bool App::messageReceived ( const ccm::Message* message )
{
        if(message->getType() == ccm::CarControlData::TYPE) {
            ccm::Message *messageCopy = messageManager()->getMessageCopy(message);
            messageCopy->setCommunicationId(ccm::SerialCommunication::TYPE);
            sendMessage(messageCopy);
        }
        return true;
}
