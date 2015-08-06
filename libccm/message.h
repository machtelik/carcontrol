#ifndef __MESSSAGE_H__
#define __MESSSAGE_H__

#include <stddef.h>
#include <stdint.h>

namespace ccm {

class Message {
  
public:
  Message();
  virtual ~Message();
  
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
  
    uint8_t getDeliveryType() const;
    void setDeliveryType(uint8_t type);
  
protected:
  
private:
    char* mData;
    uint8_t mDeliveryType;
};

} // ccm

#endif /* __MESSSAGE_H__ */
 
