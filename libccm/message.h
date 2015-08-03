#ifndef __MESSSAGE_H__
#define __MESSSAGE_H__

#include <stddef.h>

namespace ccm {

class Message {
  
public:
  Message();
  virtual ~Message();
  
  char* getData();
  const char* getData() const;
  void setDataSize(size_t size);
  size_t getDataSize() const;
  size_t getMaxDataSize() const;
  
protected:
  
private:
    char* mData;
    size_t mSize;
  
};

} // ccm

#endif /* __MESSSAGE_H__ */
 
