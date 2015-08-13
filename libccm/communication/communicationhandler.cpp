#include "communicationhandler.h"

#include <iostream>

#include "data/messagemanager.h"
#include "data/message.h"
#include "communication.h"

namespace ccm {
    
CommunicationHandler::CommunicationHandler(int sourceId ):
        mRunning(true),
        mSourceId(sourceId),
        mSendThread ( 0 ),
        mMessageManager(new MessageManager(sourceId))
{

}

CommunicationHandler::~CommunicationHandler()
{
    mRunning = false;
    
        while ( !mSendQueue.empty() ) {
                mMessageManager->release(mSendQueue.front());
                mSendQueue.pop();
        }

        while ( !mReceiveQueue.empty() ) {
                 mMessageManager->release(mReceiveQueue.front());
                mReceiveQueue.pop();
        }

        for ( int i = 0; i < mConnections.size(); ++i ) {
                delete mConnections.at ( i );
        }

        if ( mSendThread ) {
                delete mSendThread;
        }

        for ( int i = 0; i < mReceiveThreads.size(); ++i ) {
                delete mReceiveThreads.at ( i );
        }
        
        delete mMessageManager;
}

MessageManager* CommunicationHandler::messages()
{
    return mMessageManager;
}

void CommunicationHandler::getReceivedMessages ( std::queue< Message* > &messages )
{
        std::lock_guard<std::mutex> lock ( mReceiveQueueMutex );
        while ( !mReceiveQueue.empty() ) {
                messages.push ( mReceiveQueue.front() );
                mReceiveQueue.pop();
        }
}
    
    void CommunicationHandler::sendMessage ( Message* message )
{
        std::lock_guard<std::mutex> lock ( mSendQueueMutex );
        mSendQueue.push ( message );
        mSendBarrier .notify_one();
}

bool CommunicationHandler::startCommunication()
{

        mSendThread = new std::thread ( &CommunicationHandler::sendThreadFunction, this );

        return true;
}

bool CommunicationHandler::addCommunicationMethod ( Communication *communication )
{
    if(mConnections.count(communication->communicationId())) {
        std::cerr << "Communication with given id already exists" << std::endl;
    }
    
        if ( !communication->connect () ) {
                return false;
        }

        mConnections[communication->communicationId()] = communication;
        std::thread *multicastReceiveThread = new std::thread ( &CommunicationHandler::receiveThreadFunction, this, communication->communicationId() );
        mReceiveThreads[communication->communicationId()] = multicastReceiveThread;

        return true;
}

void CommunicationHandler::receiveThreadFunction ( uint8_t deliveryType )
{
        Communication *connection = mConnections[deliveryType];
        Message *message = 0;
        while ( mRunning ) {
                if ( !message ) {
                        message = messages()->getMessage();
                }

                size_t dataSize = connection->receive ( message->getData(), message->getMaxMessageSize() );
                message->setCommunicationId ( connection->communicationId() );

                if ( dataSize != message->getMessageSize() ) {
                        std::cerr << "Somethings wrong with the message size: " << dataSize << " != " << message->getMessageSize() << std::endl;
                }

                //Only handle foreign messages
                if ( mSourceId != message->getSourceId() ) {
                        std::lock_guard<std::mutex> lock ( mReceiveQueueMutex );
                        mReceiveQueue.push ( message );
                        message = 0;
                }
        }
}

void CommunicationHandler::sendThreadFunction()
{
        std::queue<Message*> messageQueue;
        while ( mRunning ) {
                {
                        std::unique_lock<std::mutex> lk ( mSendQueueMutex );
                        mSendBarrier.wait ( lk, [&]() {
                                return !CommunicationHandler::mSendQueue.empty();
                        } );

                        while ( !mSendQueue.empty() ) {
                                messageQueue.push ( mSendQueue.front() );
                                mSendQueue.pop();
                        }
                }

                while ( !messageQueue.empty() ) {
                        Message *message = messageQueue.front();
                        messageQueue.pop();
                        Communication *connection = mConnections[message->getCommunicationId()];
                        bool ok = true;
                        if ( connection ) {
                               ok = connection->send ( message->getData(), message->getMessageSize() );
                        } else {
                                std::cerr << "Did not find connection for delivery type: " << message->getCommunicationId() << ", is it enabled?" << std::endl;
                        }

                        messages()->release ( message );
                        
                        if(!ok) {
                            std::cerr << "Could not send message" << std::endl;
                        }
                        
                }

        }
        
}
    
}