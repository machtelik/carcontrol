#ifndef __MESSSAGEMANAGER_H__
#define __MESSSAGEMANAGER_H__

#include <mutex>
#include <stack>

namespace ccm {

    class Message;
    
class MessageManager {
  
public:
    MessageManager();  
     MessageManager(int defaultSourceId);    
    virtual ~MessageManager();
  
    Message *getMessage();
    Message* getMessageCopy(const Message *message);
    void release(Message *message);
  
protected:
  
private:
    
    int mDefaultSourceId;
    
    std::stack<Message*> mMessageBuffer;
    std::mutex mMessageBufferMutex;

};

} // ccm

#endif /* __MESSSAGEMANAGER_H__ */
 