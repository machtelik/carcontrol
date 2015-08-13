#include "messagemanager.h"

#include "message.h"
#include "config.h"

namespace ccm {
    
MessageManager::MessageManager ( int defaultSourceId ) :
mDefaultSourceId(defaultSourceId)
{
}

MessageManager::~MessageManager()
{
        while ( !mMessageBuffer.empty() ) {
                delete mMessageBuffer.top();
                mMessageBuffer.pop();
        }
}

Message *MessageManager::getMessage()
{
        Message *message = 0;
        
        {
                std::lock_guard<std::mutex> lock ( mMessageBufferMutex );

                if ( !mMessageBuffer.empty() ) {
                        message = mMessageBuffer.top();
                        mMessageBuffer.pop();
                }
        }

        if ( !message ) {
                message = new Message();
        }
        
        message->setSourceId(mDefaultSourceId);
        message->setCommunicationId(0);
        message->setPayloadSize(0);
        
        return message ;
}

Message* MessageManager::getMessageCopy(const Message *message)
{
    Message *copy = getMessage();
    copy->operator=(*message);
    return copy;
}

void MessageManager::release ( Message *message )
{
        std::lock_guard<std::mutex> lock ( mMessageBufferMutex );
        if(mMessageBuffer.size() < MAX_MESSAGE_BUFFER_SIZE) {
                    mMessageBuffer.push ( message );
        } else {
            delete message;
        }
}

}