#ifndef __COMMUNICATIONHANDLER_H__
#define __COMMUNICATIONHANDLER_H__

#include <stdint.h>
#include <thread>
#include <unordered_map>
#include <utility>
#include <functional>

#include "eventloop/eventloop.h"

namespace ccm
{

class Message;
class Communication;
class MessageManager;

class CommunicationHandler
{

public:

    CommunicationHandler( uint8_t sourceId );
    virtual ~CommunicationHandler();

    bool startCommunication();

    bool addCommunicationMethod( Communication *communication );
    void setMessageCallback( std::function< void( uint8_t, Message * ) > callback );

    void sendMessage( uint8_t communicationType, Message *message );

    MessageManager *messages();

private:

    void receiveThreadFunction( Communication *communication );
    void handleSendEvent( uint8_t communicationType, Message *message );

    volatile bool mRunning;

    uint8_t mSourceId;

    std::function< void( uint8_t, Message * ) > mMessageCallback;

    EventLoop *mEventLoop;

    std::thread *mSendThread;

    std::unordered_map<uint8_t, Communication *> mConnections;
    std::unordered_map<uint8_t, std::thread *> mReceiveThreads;

    MessageManager *mMessageManager;

};

} // ccm

#endif /* __COMMUNICATIONHANDLER_H__ */


