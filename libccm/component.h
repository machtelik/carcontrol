#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <chrono>

namespace ccm {
  
class Message;
class Communication;
class Messageable;
class CommunicationHandler;
  
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
 
    CommunicationHandler *communication();
      
private:
    volatile bool mRunning;
    
    std::chrono::milliseconds mLoopInterval;
    
    int8_t mId;
    
    CommunicationHandler *mCommunicationHandler;
      
    bool handleReceivedMessages();
  
};

} // ccm

#endif /* __COMPONENT_H__ */
 
