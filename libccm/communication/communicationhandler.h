#ifndef __COMMUNICATIONHANDLER_H__
#define __COMMUNICATIONHANDLER_H__

#include <stdint.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <utility>

namespace ccm {
    
    class Message;
    class Communication;
    class MessageManager;

class CommunicationHandler {
  
public:
    
    CommunicationHandler(int sourceId);
    virtual ~CommunicationHandler();
   
    bool startCommunication();
            
    bool addCommunicationMethod(Communication *communication);
    
    void sendMessage ( int communicationType, Message* message );
    
    void getReceivedMessages( std::queue< Message* > &messages);
    
     MessageManager *messages();
    
private:
    
    void receiveThreadFunction(uint8_t deliveryType);
    void sendThreadFunction();
    
    volatile bool mRunning;
    
    int mSourceId;
    
    std::thread *mSendThread;
  
    std::unordered_map<uint8_t, Communication*> mConnections;
    std::unordered_map<uint8_t, std::thread*> mReceiveThreads;
  
    std::queue< std::pair<int, Message*> > mSendQueue;
     std::queue<Message*> mReceiveQueue;

     std::condition_variable mSendBarrier;
     
    std::mutex mSendQueueMutex;
    std::mutex mReceiveQueueMutex;
    
        MessageManager *mMessageManager;
  
};

} // ccm

#endif /* __COMMUNICATIONHANDLER_H__ */
 
 
