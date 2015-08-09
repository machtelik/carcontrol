#ifndef __MESSSAGEABLE_H__
#define __MESSSAGEABLE_H__

namespace ccm {

class Message;
    
class Messageable {
  
public:
    
    virtual void fromMessage(const Message *message) = 0;
    virtual void toMessage(Message *message) const = 0;
  
protected:
  
private:

};

} // ccm

#endif /* __MESSSAGEABLE_H__ */
 
