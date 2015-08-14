#include "communication.h"

namespace ccm {
  
Communication::Communication ( uint8_t communicationType ) :
    mCommunicationType(communicationType)
{
}

uint8_t Communication::communicationType()
{
    return mCommunicationType;
}
    
}