#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "config.h"
#include "data/message.h"
#include "communication/multicastcommunication.h"

namespace ccm
{

Component::Component ( int8_t id,  int argc, char** argv ) :
        running ( false ),
        mLoopInterval ( DEFAULT_LOOP_DURATION ) ,
        mId ( id ),
        mSendThread ( 0 )
{
}

Component::~Component()
{
        while ( !mSendQueue.empty() ) {
                delete mSendQueue.front();
                mSendQueue.pop();
        }

        while ( !mReceiveQueue.empty() ) {
                delete mReceiveQueue.front();
                mReceiveQueue.pop();
        }

        while ( !mMessageBuffer.empty() ) {
                delete mMessageBuffer.top();
                mMessageBuffer.pop();
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
}

int Component::execute()
{
        if ( running ) {
                std::cerr <<  "Component already running"  << std::endl;
                return EXIT_FAILURE;
        }

        if ( !begin() ) {
                std::cerr <<  "Error while initializing component"   << std::endl;
                return EXIT_FAILURE;
        }

        if ( !startCommunication() ) {
                std::cerr <<  "Could not start communication" << std::endl;
        }

        running = true;

        while ( running ) {
                auto startTime = std::chrono::high_resolution_clock::now();

                if ( !handleMessages() ) {
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

bool Component::handleMessages()
{
        std::queue<Message*> messageQueue;
        {
                std::lock_guard<std::mutex> lock ( mReceiveQueueMutex );
                while ( !mReceiveQueue.empty() ) {
                        messageQueue.push ( mReceiveQueue.front() );
                        mReceiveQueue.pop();
                }
        }

        while ( !messageQueue.empty() ) {
                Message *message = messageQueue.front();
                messageQueue.pop();
                bool handled = messageReceived ( message );
                releaseMessage ( message );

                if ( !handled ) {
                        return false;
                }
        }

        return true;
}

void Component::exit()
{
        running = false;
}

uint8_t Component::getId()
{
    return mId;
}

Message *Component::getMessage()
{
        std::lock_guard<std::mutex> lock ( mMessageBufferMutex );

        Message *message = 0;

        if ( !mMessageBuffer.empty() ) {
                message = mMessageBuffer.top();
                mMessageBuffer.pop();
        }

        if ( !message ) {
                return new Message();
        }

        //Set default communication data
        message->setSourceId(mId);
        message->setCommunicationId(MulticastCommunication::TYPE);
        message->setPayloadSize(0);
        
        return message ;
}

void Component::sendMessage ( ccm::Message* message )
{
        std::lock_guard<std::mutex> lock ( mSendQueueMutex );
        mSendQueue.push ( message );
        mSendBarrier .notify_one();
}

void Component::releaseMessage ( Message *message )
{
        std::lock_guard<std::mutex> lock ( mMessageBufferMutex );
        mMessageBuffer.push ( message );
}

bool Component::startCommunication()
{

        if ( !addCommunicationMethod(new MulticastCommunication ( MULTICAST_IP, MULTICAST_PORT ))) {
                return false;
        }

        mSendThread = new std::thread ( &Component::sendThreadFunction, this );

        return true;
}

bool Component::addCommunicationMethod ( Communication *communication )
{
    if(mConnections.count(communication->communicationId())) {
        std::cerr << "Communication with given id already exists" << std::endl;
    }
    
        if ( !communication->connect () ) {
                return false;
        }

        mConnections[communication->communicationId()] = communication;
        std::thread *multicastReceiveThread = new std::thread ( &Component::receiveThreadFunction, this, communication->communicationId() );
        mReceiveThreads[communication->communicationId()] = multicastReceiveThread;

        return true;
}

void Component::receiveThreadFunction ( uint8_t deliveryType )
{
        Communication *connection = mConnections[deliveryType];
        Message *message = 0;
        while ( running ) {
                if ( !message ) {
                        message = getMessage();
                }

                size_t dataSize = connection->receive ( message->getData(), message->getMaxMessageSize() );
                message->setCommunicationId ( connection->communicationId() );

                if ( dataSize != message->getMessageSize() ) {
                        std::cerr << "Somethings wrong with the message size: " << dataSize << " != " << message->getMessageSize() << std::endl;
                }

                //Only handle foreign messages
                if ( mId != message->getSourceId() ) {
                        std::lock_guard<std::mutex> lock ( mReceiveQueueMutex );
                        mReceiveQueue.push ( message );
                        message = 0;
                }
        }
}

void Component::sendThreadFunction()
{
        std::queue<Message*> messageQueue;
        while ( running ) {
                {
                        std::unique_lock<std::mutex> lk ( mSendQueueMutex );
                        mSendBarrier.wait ( lk, [&]() {
                                return !Component::mSendQueue.empty();
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

                        releaseMessage ( message );
                        
                        if(!ok) {
                            std::cerr << "Could not send message" << std::endl;
                        }
                        
                }

        }
        
}

}


