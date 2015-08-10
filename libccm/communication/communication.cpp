#include "communication.h"

namespace ccm {
  
Communication::Communication ( uint8_t communicationId ) :
    mCommunicationId(communicationId)
{
}

uint8_t Communication::communicationId()
{
    return mCommunicationId;
}
    
}