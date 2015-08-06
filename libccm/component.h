#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <chrono>
#include <queue>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ccm {
  
class Multicast;
class Message;
  
class Component {
  
public:
  Component(int8_t id , int argc, char** argv);
  virtual ~Component();

  int execute();
  
  void exit();
  
protected:
  
    virtual bool begin() = 0;
    virtual bool loop() = 0;
    virtual bool messageReceived(const Message *message) = 0;
  
    Message *getMessage();
    void sendMessage(Message *message);
    void releaseMessage(Message *message);
  
private:
    volatile bool running;
    
    int8_t mId;
    
    std::thread *mReceiveThread;
    std::thread *mSendThread;
  
    Multicast *mMulticastConnection;
    int mPort;
    std::string mIp;
  
    std::queue<Message*> *mSendQueue;
     std::queue<Message*> *mReceiveQueue;
    std::stack<Message*> *mMessageBuffer;
    
     std::condition_variable mSendBarrier;
     
    std::mutex mSendQueueMutex;
    std::mutex mReceiveQueueMutex;
    std::mutex mMessageBufferMutex;
   
    std::chrono::milliseconds mLoopInterval;
  
    bool startWorkerThreads();
    void receiveFunction();
    bool sendFunction();
  
};

} // ccm

#endif /* __COMPONENT_H__ */
 
