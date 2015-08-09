#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <chrono>
#include <queue>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

namespace ccm {
  
class Message;
class Communication;
class Messageable;
  
class Component {
  
public:
  Component(int8_t id , int argc, char** argv);
  virtual ~Component();

  int execute();
  
  void exit();
  
  uint8_t getId();
  
protected:
  
    virtual bool begin() = 0;
    virtual bool loop() = 0;
    virtual bool messageReceived(const Message *message) = 0;
  
    Message *getMessage();
    void sendMessage(Message *message);
    void releaseMessage(Message *message);
    
    bool addCommunicationMethod(Communication *communication);
    
    void send(const Messageable *messageable);
    void addMessageReceiver(uint8_t messageType, Messageable *messageable);
    void removeMessageReceiver(Messageable *messageable);
  
private:
    volatile bool running;
    
    std::chrono::milliseconds mLoopInterval;
    
    int8_t mId;
    
    std::thread *mSendThread;
  
    std::unordered_map<uint8_t, Communication*> mConnections;
    std::unordered_map<uint8_t, std::thread*> mReceiveThreads;
  
    std::queue<Message*> mSendQueue;
     std::queue<Message*> mReceiveQueue;
    std::stack<Message*> mMessageBuffer;
    
     std::condition_variable mSendBarrier;
     
    std::mutex mSendQueueMutex;
    std::mutex mReceiveQueueMutex;
    std::mutex mMessageBufferMutex;
  
    bool startCommunication();
    void receiveThreadFunction(uint8_t deliveryType);
    void sendThreadFunction();
    
    bool handleMessages();
  
};

} // ccm

#endif /* __COMPONENT_H__ */
 
