#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <mutex>
#include <condition_variable>

#include "eventloop/eventloop.h"

namespace ccm
{

class Message;
class Communication;
class Messageable;
class CommunicationHandler;
class PeriodicTimer;

class Component : public EventLoop
{

public:
    Component( int8_t id , int argc, char **argv );
    virtual ~Component();

    int execute();

    void exit();

    uint8_t getId();

protected:

    virtual bool begin() = 0;
    virtual bool loop() = 0;
    virtual bool messageReceived( uint8_t communicationType, const Message *message ) = 0;

    CommunicationHandler *communication();
    void disableLoop();

private:
    volatile bool mRunning;

    PeriodicTimer *mLoopTimer;
    bool mLoopEnabled;
    uint32_t mLoopInterval;

    int8_t mId;

    CommunicationHandler *mCommunicationHandler;

    void handleLoopEvent();
    void handleMessageEvent( uint8_t communicationType, Message *message );
    void postMessage( uint8_t communicationType, Message *message );


};

} // ccm

#endif /* __COMPONENT_H__ */

