#include "eventloop.h"

#include <iostream>

namespace ccm
{

EventLoop::EventLoop() :
    mRunning( false )
{

}

EventLoop::~EventLoop()
{

}


int EventLoop::execute()
{
    if( mRunning ) {
        return EXIT_FAILURE;
    }

    mRunning = true;
    std::queue< std::function<void()> > eventQueue;

    while( mRunning ) {
        {
            std::unique_lock<std::mutex> lk( mEventMutex );
            mEventBarrier.wait( lk, [&]() {
                return !EventLoop::mEventQueue.empty();
            } );

            while( !mEventQueue.empty() ) {
                eventQueue.push( mEventQueue.front() );
                mEventQueue.pop();
            }
        }

        while( !eventQueue.empty() ) {
            auto event = eventQueue.front();
            eventQueue.pop();

            event();

        }

    }

    return EXIT_SUCCESS;

}

void EventLoop::exit()
{
    mRunning = false;
    mEventBarrier.notify_one();
}

void EventLoop::post( std::function<void()> event )
{
    std::lock_guard<std::mutex> lock( mEventMutex );
    mEventQueue.push( event );
    mEventBarrier.notify_one();
}


}