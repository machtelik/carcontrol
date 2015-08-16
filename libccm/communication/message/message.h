#ifndef __MESSSAGE_H__
#define __MESSSAGE_H__

#include <stddef.h>
#include <stdint.h>

namespace ccm {
    
class Message {
  
public:
  Message();
  virtual ~Message();
  
  Message& operator=(const Message &other);
  
  uint8_t getType() const;
  void setType(uint8_t type);
  
   uint8_t getSourceId() const;
  void setSourceId(uint8_t type);
  
  char* getPayload();
  const char* getPayload() const;
  
  void setPayloadSize(uint16_t size);
  uint16_t getPayloadSize() const;
  static uint16_t getMaxPayloadSize();
  
  uint16_t getMessageSize() const;
  static uint16_t getMaxMessageSize();
  
  char* getData();
  const char* getData() const;
  
protected:
  
private:
    char* mData;

};

} // ccm

#endif /* __MESSSAGE_H__ */
 
