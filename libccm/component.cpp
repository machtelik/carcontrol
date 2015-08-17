#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <time.h>

#include "config.h"
#include "communication/message/message.h"
#include "communication/types/multicastcommunication.h"
#include "communication/communicationhandler.h"
#include "communication/message/messagemanager.h"
#include "util/periodictimer.h"

namespace ccm
{

Component::Component( int8_t id,  int argc, char **argv ) :
    mRunning( false ),
    mLoopInterval( DEFAULT_LOOP_DURATION ),
    mCommunicationHandler( new CommunicationHandler( id ) )
{
}

Component::~Component()
{
    exit();
}


int Component::execute()
{
    if( mRunning ) {
        std::cerr <<  "Component already running"  << std::endl;
        return EXIT_FAILURE;
    }

    if( !begin() ) {
        std::cerr <<  "Error while initializing component"   << std::endl;
        return EXIT_FAILURE;
    }

    mCommunicationHandler->addCommunicationMethod( new MulticastCommunication( MULTICAST_IP, MULTICAST_PORT ) );

    if( !mCommunicationHandler->startCommunication() ) {
        std::cerr <<  "Could not start communication" << std::endl;
    }

    mRunning = true;

    PeriodicTimer loop( mLoopInterval, [&]() {
        Component::handleLoop();
    } );

    std::unique_lock<std::mutex> lk( mWaitMutex );
    mWaitBarrier.wait( lk, [&]() {
        return mRunning;
    } );

    return EXIT_SUCCESS;
}

void Component::handleLoop()
{
    if( !handleReceivedMessages() ) {
        std::cerr <<  "Error handeling message"   << std::endl;
        exit();
    }

    if( !loop() ) {
        std::cerr <<  "Error while execution loop"   << std::endl;
        exit();
    }
}

bool Component::handleReceivedMessages()
{
    std::queue<Message *> messageQueue;
    communication()->getReceivedMessages( messageQueue );

    while( !messageQueue.empty() ) {
        Message *message = messageQueue.front();
        messageQueue.pop();
        bool handled = messageReceived( message );
        communication()->messages()->release( message );

        if( !handled ) {
            return false;
        }
    }

    return true;
}

void Component::exit()
{
    mRunning = false;
    mWaitBarrier.notify_all();
    if( mCommunicationHandler ) {
        delete mCommunicationHandler;
        mCommunicationHandler = 0;
    }
}

uint8_t Component::getId()
{
    return mId;
}

CommunicationHandler *Component::communication()
{
    return mCommunicationHandler;
}

}


