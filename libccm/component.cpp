#include "component.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

#include "config.h"
#include "message.h"
#include "multicast.h"

namespace ccm {

Component::Component (int8_t id,  int argc, char** argv ) :
    mLoopInterval ( DEFAULT_LOOP_DURATION) ,
    mSendThread(0),
    mReceiveThread(0),
    running(false),
    mSendQueue(new std::queue<Message*>()),
    mReceiveQueue(new std::queue<Message*>()),
    mMessageBuffer(new std::stack<Message*>()),
    mMulticastConnection(new Multicast()),
    mIp(DEFAULT_IP),
    mPort(DEFAULT_PORT),
    mId(id)
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

    if ( !begin() ) {
        std::cerr <<  "Error while initializing component"   << std::endl;
        return EXIT_FAILURE;
    }

    if ( !mMulticastConnection->connect ( mIp, mPort ) ) {
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
        
        {
            std::lock_guard<std::mutex> lock(mReceiveQueueMutex);
            while(!mReceiveQueue->empty()) {
                messageQueue.push(mReceiveQueue->front());
                mReceiveQueue->pop();
            }
        }
	
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
    std::lock_guard<std::mutex> lock(mMessageBufferMutex);
    
    Message *message = 0;
    
    if (!mMessageBuffer->empty() ) {
	message = mMessageBuffer->top();
	mMessageBuffer->pop();
    }
    
    if (message ) {
        return message;
    } 
    
    return new Message() ;
}

void Component::sendMessage ( Message *message ) {
    std::lock_guard<std::mutex> lock(mSendQueueMutex);
    mSendQueue->push ( message );
   mSendBarrier .notify_one();
}

void Component::releaseMessage ( Message *message ) {
    std::lock_guard<std::mutex> lock(mMessageBufferMutex);
    mMessageBuffer->push ( message );
}

bool Component::startWorkerThreads() {
    
    std::cout <<  "Starting worker"   << std::endl;
    
   mSendThread = new std::thread ( &Component::sendFunction, this );
    mReceiveThread = new std::thread ( &Component::receiveFunction, this );

    return true;
}

void Component::receiveFunction() {
    Message *message = 0;
    while ( running ) {
        if (!message) {
            message = getMessage();
        }

        size_t dataSize = mMulticastConnection->receive ( message->getData(), message->getMaxMessageSize() );
        
        if(dataSize != message->getMessageSize()) {
            std::cerr << "Somethings wrong with the message size: " << dataSize << " != " << message->getMessageSize() << std::endl;
        }
        
        //Only handle foreign messages
        if (mId != message->getSourceId()) {
            std::lock_guard<std::mutex> lock(mReceiveQueueMutex);
            mReceiveQueue->push(message);
            message = 0;
        }
    }
}

bool Component::sendFunction() {
    std::queue<Message*> messageQueue;
    while ( running ) {
        {
            std::unique_lock<std::mutex> lk(mSendQueueMutex);
            mSendBarrier.wait(lk, [&](){return !Component::mSendQueue->empty();});
            
            while(!mSendQueue->empty()) {
                messageQueue.push(mSendQueue->front());
                mSendQueue->pop();
            }
        }
           
        while ( !messageQueue.empty() ) {
            Message *message = messageQueue.front();
            messageQueue.pop();
            message->setSourceId(mId);
            mMulticastConnection->send ( message->getData(), message->getMessageSize() );
            releaseMessage ( message );
        }
        
    }
}

}
