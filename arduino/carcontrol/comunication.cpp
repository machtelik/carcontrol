#include "communication.h"

#include <Arduino.h>

#include "config.h"


void Communication::init()
{
    Serial.begin( SERIAL_SPEED );
}

void Communication::send( const char *data, uint16_t length )
{
    if( length > MESSAGE_MAX_SIZE ) {
        return;
    }

    Serial.write( MESSAGE_START );

    for( int i = 0; i < length; ++i ) {
        if( data[i] == MESSAGE_START ) {
            Serial.write( MESSAGE_ESCAPE );
            Serial.write( MESSAGE_START_ESCAPED );
        } else if( data[i] == MESSAGE_END ) {
            Serial.write( MESSAGE_ESCAPE );
            Serial.write( MESSAGE_END_ESCAPED );
        } else if( data[i] == MESSAGE_ESCAPE ) {
            Serial.write( MESSAGE_ESCAPE );
            Serial.write( MESSAGE_ESCAPE_ESCAPED );
        } else {
            Serial.write( data[i] );
        }
    }

    Serial.write( MESSAGE_END );
}

void Communication::receive( void(*dispatch)(char*, uint16_t) )
{
    //Try to read a complete message
    while( Serial.available() ) {
        char readData = Serial.read();

        switch( readData ) {
        case MESSAGE_START:
            mReadDataBytes = 0;
            mReadingData = true;
            mReadEscape = false;
            break;

        case MESSAGE_ESCAPE:
            if( mReadingData && !mReadEscape ) {
                mReadEscape = true;
            } else {
                mReadingData = false;
            }
            break;

        case MESSAGE_END:
            if( mReadingData && !mReadEscape ) {
                dispatch(mReceiveBuffer, mReadDataBytes);
                mReadingData = false;
            } else {
                mReadingData = false;
            }
            break;

        case MESSAGE_START_ESCAPED:
            if( mReadingData && mReadEscape ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_START;
                mReadEscape = false;
            } else {
                mReadingData = false;
            }
            break;

        case MESSAGE_ESCAPE_ESCAPED:
            if( mReadingData && mReadEscape ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_ESCAPE;
                mReadEscape = false;
            } else {
                mReadingData = false;
            }
            break;

        case MESSAGE_END_ESCAPED:
            if( mReadingData && mReadEscape ) {
                mReceiveBuffer[mReadDataBytes++] = MESSAGE_END;
                mReadEscape = false;
            } else {
                mReadingData = false;
            }
            break;


        default:
            if(mReadDataBytes > MESSAGE_MAX_SIZE) {
                mReadingData = false;
            } else if( mReadingData ) {
                mReceiveBuffer[mReadDataBytes++] = readData;
            }
            break;
        }
    }
}

int Communication::mReadDataBytes = 0;
bool Communication::mReadingData = false;
bool Communication::mReadEscape = false;
char Communication::mReceiveBuffer[MESSAGE_MAX_SIZE];
char Communication::mSendBuffer[MESSAGE_MAX_SIZE];
