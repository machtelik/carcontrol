#include "communicationhandler.h"

#include <iostream>

#include "communication/message/messagemanager.h"
#include "communication/message/message.h"
#include "communication.h"
#include "config.h"

namespace ccm
{

CommunicationHandler::CommunicationHandler( int sourceId ):
    mRunning( true ),
    mSourceId( sourceId ),
    mSendThread( 0 ),
    mMessageManager( new MessageManager( sourceId ) )
{

}

CommunicationHandler::~CommunicationHandler()
{
    mRunning = false;

    while( !mSendQueue.empty() ) {
        mMessageManager->release( mSendQueue.front().second );
        mSendQueue.pop();
    }

    while( !mReceiveQueue.empty() ) {
        mMessageManager->release( mReceiveQueue.front().second );
        mReceiveQueue.pop();
    }

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

void CommunicationHandler::getReceivedMessages( std::queue< std::pair< uint8_t, ccm::Message * > > &messages )
{
    std::lock_guard<std::mutex> lock( mReceiveQueueMutex );
    while( !mReceiveQueue.empty() ) {
        messages.push( mReceiveQueue.front() );
        mReceiveQueue.pop();
    }
}

void CommunicationHandler::sendMessage( int communicationType, Message *message )
{
    std::lock_guard<std::mutex> lock( mSendQueueMutex );
    mSendQueue.push( std::pair<int, Message *>( communicationType, message ) );
    mSendBarrier .notify_one();
}

bool CommunicationHandler::startCommunication()
{

    mRunning = true;
    mSendThread = new std::thread( &CommunicationHandler::sendThreadFunction, this );

    return true;
}

bool CommunicationHandler::addCommunicationMethod( Communication *communication )
{
    if( mConnections.count( communication->communicationType() ) ) {
        std::cerr << "Communication with given id already exists" << std::endl;
    }

    if( !communication->connect() ) {
        return false;
    }

    mConnections[communication->communicationType()] = communication;
    std::thread *multicastReceiveThread = new std::thread( &CommunicationHandler::receiveThreadFunction, this, communication->communicationType() );
    mReceiveThreads[communication->communicationType()] = multicastReceiveThread;

    return true;
}

void CommunicationHandler::receiveThreadFunction( uint8_t deliveryType )
{
    Communication *connection = mConnections[deliveryType];
    Message *message = 0;
    while( mRunning ) {
        if( !message ) {
            message = messages()->getMessage();
        }

        size_t dataSize = connection->receive( message->getData(), message->getMaxMessageSize() );

        if( dataSize != message->getMessageSize() ) {
            std::cerr << "Somethings wrong with the message size: " << dataSize << " != " << message->getMessageSize() << std::endl;
            continue;
        }

        //Only handle foreign messages
        if( mSourceId != message->getSourceId() ) {
            std::lock_guard<std::mutex> lock( mReceiveQueueMutex );
            mReceiveQueue.push( std::pair<uint8_t, Message *>(deliveryType, message) );
            message = 0;

            //If the buffer is too large recycle the oldest message while we have the lock
            if( mReceiveQueue.size() >= MAX_MESSAGE_BUFFER_SIZE ) {
                message = mReceiveQueue.front().second;
                mReceiveQueue.pop();
            }
        }
    }
}

void CommunicationHandler::sendThreadFunction()
{
    std::queue< std::pair<int, Message *> > messageQueue;
    while( mRunning ) {
        {
            std::unique_lock<std::mutex> lk( mSendQueueMutex );
            mSendBarrier.wait( lk, [&]() {
                return !CommunicationHandler::mSendQueue.empty();
            } );

            while( !mSendQueue.empty() ) {
                messageQueue.push( mSendQueue.front() );
                mSendQueue.pop();
            }
        }

        while( !messageQueue.empty() ) {
            auto messageData = messageQueue.front();
            messageQueue.pop();
            Message *message = messageData.second;
            Communication *connection = mConnections[messageData.first];

            message->setSourceId(mSourceId);
            
            bool ok = true;
            if( connection ) {
                ok = connection->send( message->getData(), message->getMessageSize() );
            } else {
                std::cerr << "Did not find connection for delivery type: " << messageData.first << ", is it enabled?" << std::endl;
            }

            messages()->release( message );

            if( !ok ) {
                std::cerr << "Could not send message" << std::endl;
            }

        }

    }

}

}