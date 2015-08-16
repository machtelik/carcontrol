#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "config.h"
#include "communication/message/message.h"
#include "communication/types/multicastcommunication.h"
#include "communication/communicationhandler.h"
#include "communication/message/messagemanager.h"

namespace ccm
{

Component::Component ( int8_t id,  int argc, char** argv ) :
        mRunning ( false ),
        mLoopInterval ( DEFAULT_LOOP_DURATION ),
        mCommunicationHandler(new CommunicationHandler(id))
{
}

Component::~Component()
{
}

int Component::execute()
{
        if ( mRunning ) {
                std::cerr <<  "Component already running"  << std::endl;
                return EXIT_FAILURE;
        }

        if ( !begin() ) {
                std::cerr <<  "Error while initializing component"   << std::endl;
                return EXIT_FAILURE;
        }
        
        mCommunicationHandler->addCommunicationMethod(new MulticastCommunication(MULTICAST_IP, MULTICAST_PORT));

        if ( !mCommunicationHandler->startCommunication() ) {
                std::cerr <<  "Could not start communication" << std::endl;
        }

        mRunning = true;

        while ( mRunning ) {
                auto startTime = std::chrono::high_resolution_clock::now();

                if ( !handleReceivedMessages() ) {
                        std::cerr <<  "Error handeling message"   << std::endl;
                        return EXIT_FAILURE;
                }

                if ( !loop() ) {
                        std::cerr <<  "Error while execution loop"   << std::endl;
                        return EXIT_FAILURE;
                }

                auto dT = std::chrono::high_resolution_clock::now() - startTime;
                std::this_thread::sleep_for ( mLoopInterval - dT );
        }

        return EXIT_SUCCESS;
}

bool Component::handleReceivedMessages()
{
        std::queue<Message*> messageQueue;
        communication()->getReceivedMessages(messageQueue);

        while ( !messageQueue.empty() ) {
                Message *message = messageQueue.front();
                messageQueue.pop();
                bool handled = messageReceived ( message );
               communication()->messages()->release ( message );

                if ( !handled ) {
                        return false;
                }
        }

        return true;
}

void Component::exit()
{
        mRunning = false;
}

uint8_t Component::getId()
{
    return mId;
}

CommunicationHandler* Component::communication()
{
    return mCommunicationHandler;
}

}


