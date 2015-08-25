#include "communication.h"

#include <Arduino.h>

#include "config.h"

namespace Communication {
    
int mReadDataBytes = 0;
char mReceiveBuffer[MESSAGE_MAX_SIZE];
char mSendBuffer[MESSAGE_MAX_SIZE];

enum ReceiveState {WAIT_FOR_START = 0, READING_CHAR = 1, READING_ESCAPE = 2} mState = WAIT_FOR_START;

void setup()
{
    Serial.begin( SERIAL_SPEED );
}

char* sendBuffer()
{
    return mSendBuffer;
}

void send( const char *data, uint16_t length )
{
    if( length > MESSAGE_MAX_SIZE ) {
        return;
    }

    Serial.write( MESSAGE_START );

    for( int i = 0; i < length; ++i ) {
        switch(data[i]) {
            case MESSAGE_START:
                Serial.write( MESSAGE_ESCAPE );
                Serial.write( MESSAGE_START_ESCAPED );
                break;
                
            case MESSAGE_END:
                Serial.write( MESSAGE_ESCAPE );
                Serial.write( MESSAGE_END_ESCAPED );
                break;
                
            case MESSAGE_ESCAPE:
                Serial.write( MESSAGE_ESCAPE );
                Serial.write( MESSAGE_ESCAPE_ESCAPED );
                break;
                
            default:
                Serial.write( data[i] );
                break;
        }
    }

    Serial.write( MESSAGE_END );
}

void receive( void(*dispatch)(char*, uint16_t) )
{
    while( Serial.available() ) {
        char readData = Serial.read();
        
        if (mReadDataBytes > MESSAGE_MAX_SIZE) {
            mState = WAIT_FOR_START;
        }
        
        switch( readData ) {
        case MESSAGE_START:
            mReadDataBytes = 0;
            mState = READING_CHAR;
            break;

        case MESSAGE_ESCAPE:
            if( mState == READING_CHAR ) {
                mState = READING_ESCAPE;
            } else {
                mState = WAIT_FOR_START;
            }
            break;

        case MESSAGE_END:
            if( mState == READING_CHAR ) {
                dispatch(mReceiveBuffer, mReadDataBytes);
            }
            mState = WAIT_FOR_START;
            break;

        case MESSAGE_START_ESCAPED:
            if( mState == READING_ESCAPE ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_START;
                mState = READING_CHAR;
            } else {
                mState = WAIT_FOR_START;
            }
            break;

        case MESSAGE_ESCAPE_ESCAPED:
            if( mState == READING_ESCAPE ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_ESCAPE;
                mState = READING_CHAR;
            } else {
                mState = WAIT_FOR_START;
            }
            break;

        case MESSAGE_END_ESCAPED:
            if( mState == READING_ESCAPE ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_END;
                mState = READING_CHAR;
            } else {
                mState = WAIT_FOR_START;
            }
            break;

        default:
            if(mState == READING_CHAR) {
                mReceiveBuffer[mReadDataBytes++] = readData;
            } else {
                mState = WAIT_FOR_START; 
            }
            break;
        }
    }
}

}
