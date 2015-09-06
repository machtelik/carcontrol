#include "communicationhandler.h"

#include <iostream>

#include "communication/message/messagemanager.h"
#include "communication/message/message.h"
#include "communication.h"
#include "config.h"

namespace ccm
{

CommunicationHandler::CommunicationHandler( uint8_t sourceId ):
    mEventLoop( new EventLoop() ),
    mRunning( true ),
    mSourceId( sourceId ),
    mSendThread( 0 ),
    mMessageManager( new MessageManager() )
{

}

CommunicationHandler::~CommunicationHandler()
{
    mRunning = false;

    delete mEventLoop;

    for( int i = 0; i < mConnections.size(); ++i ) {
        delete mConnections.at( i );
    }

    if( mSendThread ) {
        delete mSendThread;
    }

    for( int i = 0; i < mReceiveThreads.size(); ++i ) {
        mReceiveThreads.at( i )->join();
        delete mReceiveThreads.at( i );
    }

    delete mMessageManager;
}

MessageManager *CommunicationHandler::messages()
{
    return mMessageManager;
}

bool CommunicationHandler::startCommunication()
{

    mRunning = true;

    for( auto communication : mConnections ) {
        if( !communication.second->connect() ) {
            return false;
        }
        mReceiveThreads[communication.first] = new std::thread( &CommunicationHandler::receiveThreadFunction, this, communication.second );
    }

    mSendThread = new std::thread( &EventLoop::execute, mEventLoop );

    return true;
}

bool CommunicationHandler::addCommunicationMethod( Communication *communication )
{
    if( mRunning ) {
        std::cerr << "Cannot add communication method while running" << std::endl;
        return false;
    }

    if( mConnections.count( communication->communicationType() < 1 ) ) {
        std::cerr << "Communication with given id already exists" << std::endl;
        return false;
    }

    mConnections[communication->communicationType()] = communication;

    return true;
}

void CommunicationHandler::setMessageCallback( std::function< void( uint8_t, Message * ) > callback )
{
    mMessageCallback = callback;
}

void CommunicationHandler::receiveThreadFunction( Communication *communication )
{
    Message *message = 0;
    while( mRunning ) {

        if( !message ) {
            message = messages()->getMessage();
        }

        size_t dataSize = communication->receive( message->getData(), message->getMaxMessageSize() );

        if( dataSize != message->getMessageSize() ) {
            std::cerr << "Somethings wrong with the message size: " << dataSize << " != " << message->getMessageSize() << std::endl;
            continue;
        }

        //Only handle foreign messages
        if( mMessageCallback && mSourceId != message->getSourceId() ) {
            mMessageCallback( communication->communicationType(), message );
            message = 0;
        }

    }
}

void CommunicationHandler::sendMessage( uint8_t communicationType, Message *message )
{
    mEventLoop->post( [ = ] {
        handleSendEvent( communicationType, message );
    } );
}

void CommunicationHandler::handleSendEvent( uint8_t communicationType, Message *message )
{
    Communication *connection = mConnections[communicationType];

    message->setSourceId( mSourceId );

    if( !connection ) {
        std::cerr << "Did not find communication type: " << communicationType << ", is it enabled?" << std::endl;
        messages()->release( message );
        return;
    }

    if( !connection->send( message->getData(), message->getMessageSize() ) ) {
        std::cerr << "Could not send message" << std::endl;
    }

    messages()->release( message );
}

}