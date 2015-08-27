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
    EventLoop(),
    mRunning( false ),
    mLoopInterval( DEFAULT_LOOP_DURATION ),
    mCommunicationHandler( new CommunicationHandler( id ) )
{
    mCommunicationHandler->setMessageCallback( [this]( uint8_t communicationType, Message * message ) {
        postMessage( communicationType, message );
    } );
}

Component::~Component()
{
    exit();
    if( mCommunicationHandler ) {
        delete mCommunicationHandler;
    }
}

void Component::postMessage( uint8_t communicationType, Message *message )
{
    post( [ = ] {
        handleMessageEvent( communicationType, message );
    } );
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

    PeriodicTimer loop( mLoopInterval, [this] {
        post( [this]{
            handleLoopEvent();
        } );
    } );

    return EventLoop::execute();
}

void Component::handleLoopEvent()
{
    if( !loop() ) {
        std::cerr <<  "Error handeling loop" << std::endl;
        exit();
    }
}

void Component::handleMessageEvent( uint8_t communicationType, Message *message )
{
    bool handled = messageReceived( communicationType, message );
    communication()->messages()->release( message );

    if( !handled ) {
        std::cerr <<  "Error handeling message"   << std::endl;
        exit();
    }
}

void Component::exit()
{
    mRunning = false;
    EventLoop::exit();
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


