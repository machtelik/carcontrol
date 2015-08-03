#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "config.h"
#include "message.h"
#include "multicast.h"

namespace ccm {

Component::Component ( int argc, char** argv ) :
    mLoopInterval ( DEFAULT_LOOP_DURATION) ,
    mSendThread(0),
    mReceiveThread(0),
    running(false),
    mSendQueue(new std::queue<Message*>()),
    mReceiveQueue(new std::queue<Message*>()),
    mMessageBuffer(new std::stack<Message*>()),
    mMulticastConnection(new Multicast()),
    mSendQueueMutex(new std::mutex()),
    mReceiveQueueMutex(new std::mutex()),
    mMessageBufferMutex(new std::mutex()),
    mIp(DEFAULT_IP),
    mPort(DEFAULT_PORT)
{
}

Component::~Component()
{
    while(!mSendQueue->empty()) {
	delete mSendQueue->front();
	mSendQueue->pop();
    }
    delete mSendQueue;
    
    while(!mReceiveQueue->empty()) {
	delete mReceiveQueue->front();
	mReceiveQueue->pop();
    }
    delete mReceiveQueue;
    
    while(!mMessageBuffer->empty()) {
	delete mMessageBuffer->top();
	mMessageBuffer->pop();
    }
    delete mMessageBuffer;
    
    delete mMulticastConnection;
    
    delete mSendQueueMutex;
    delete mMessageBufferMutex;
    
    if(mSendThread) {
	delete mSendThread;
    }
    if(mReceiveThread) {
	delete mReceiveThread;
    }
}

int Component::execute() {
    if(running) {
	std::cerr <<  "Component already running"  << std::endl;
        return EXIT_FAILURE;
    }
    
    if ( !mMulticastConnection->connect ( mIp, mPort ) ) {
        return EXIT_FAILURE;
    }

    if ( !begin() ) {
        std::cerr <<  "Error while initializing component"   << std::endl;
        return EXIT_FAILURE;
    }

    if ( !startWorkerThreads() ) {
        std::cerr <<  "Could not start worker threads" << std::endl;
    }

    running = true;
    
    std::queue<Message*> messageQueue;
    
    while ( running ) {
        auto startTime = std::chrono::high_resolution_clock::now();

        if ( !loop() ) {
            std::cerr <<  "Error while execution loop"   << std::endl;
            return EXIT_FAILURE;
        }
        
        mReceiveQueueMutex->lock();
	while(!mReceiveQueue->empty()) {
	    messageQueue.push(mReceiveQueue->front());
	    mReceiveQueue->pop();
	}
	mReceiveQueueMutex->unlock();
	
	while(!messageQueue.empty()) {
	    Message *message = messageQueue.front();
	    messageQueue.pop();
	    bool handled = messageReceived(message);
	    releaseMessage(message);
	    
	    if(!handled) {
		std::cerr <<  "Error handeling message"   << std::endl;
		return false;
	    }
	}

        auto dT = std::chrono::high_resolution_clock::now() - startTime;
        std::this_thread::sleep_for ( mLoopInterval - dT );
    }
    
    return EXIT_SUCCESS;
}

void Component::exit() {
    running = false;
     mSendThread->join();
    mReceiveThread->join();
    mMulticastConnection->disconnect();
}

Message *Component::getMessage() {
    Message *message = 0;

    mMessageBufferMutex->lock();
    if (!mMessageBuffer->empty() ) {
	message = mMessageBuffer->top();
	mMessageBuffer->pop();
    }
    mMessageBufferMutex->unlock();
    
    if (message ) {
        return message;
    } 
    
    return new Message() ;
}

void Component::sendMessage ( Message *message ) {
    mSendQueueMutex->lock();
    mSendQueue->push ( message );
    mSendQueueMutex->unlock();
}

void Component::releaseMessage ( Message *message ) {
    mMessageBufferMutex->lock();
    mMessageBuffer->push ( message );
    mMessageBufferMutex->unlock();
}

bool Component::startWorkerThreads() {
    
    std::cout <<  "Starting worker"   << std::endl;
    
   mSendThread = new std::thread ( &Component::sendFunction, this );
    mReceiveThread = new std::thread ( &Component::receiveFunction, this );

    return true;
}

void Component::receiveFunction() {
    while ( running ) {
        Message *message = getMessage();

        size_t dataSize = mMulticastConnection->receive ( message->getData(), message->getMaxDataSize() );
        message->setDataSize ( dataSize );

        mReceiveQueueMutex->lock();
        mReceiveQueue->push(message);
	mReceiveQueueMutex->unlock();
    }
}

bool Component::sendFunction() {
    std::queue<Message*> messageQueue;
    while ( running ) {
	mSendQueueMutex->lock();
	while(!mSendQueue->empty()) {
	    messageQueue.push(mSendQueue->front());
	    mSendQueue->pop();
	}
	mSendQueueMutex->unlock();
	
        while ( !messageQueue.empty() ) {
            Message *message = messageQueue.front();
            messageQueue.pop();
            mMulticastConnection->send ( message->getData(), message->getDataSize() );
            releaseMessage ( message );
        }
        
        std::this_thread::yield();
    }
}

}
