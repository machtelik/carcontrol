#include "serialcommunication.h"

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "config.h"

namespace ccm
{

static const uint8_t MESSAGE_START = 0;
static const uint8_t MESSAGE_END = 1;
static const uint8_t MESSAGE_ESCAPE = 2;
static const uint8_t MESSAGE_START_ESCAPED = 3;
static const uint8_t MESSAGE_END_ESCAPED = 4;
static const uint8_t MESSAGE_ESCAPE_ESCAPED = 5;

SerialCommunication::SerialCommunication( const std::string &device, int baudRate ) :
    Communication( TYPE ),
    mSocketDesc( -1 ),
    mDevice( device ),
    mBaudRate( baudRate )
{
}

SerialCommunication::~SerialCommunication()
{
    disconnect();
}

bool SerialCommunication::connect()
{
    if( mSocketDesc != -1 ) {
        std::cerr << "Already connected"  << std::endl;
        return false;
    }

    mSocketDesc = createSocket( mDevice );

    if( mSocketDesc == -1 ) {
        std::cerr << "Could not create socket"  << std::endl;
        return false;
    }

    setupSocket( mSocketDesc, mBaudRate, 0 );
}

bool SerialCommunication::disconnect()
{
    if( mSocketDesc == -1 ) {
        return false;
    }

    if( !close( mSocketDesc ) ) {
        return false;
    }

    mSocketDesc = -1;
}

int SerialCommunication::createSocket( const std::string &device )
{
    int socketDesc = open( device.c_str(), O_RDWR | O_NOCTTY | O_SYNC );
    if( socketDesc == -1 ) {
        return -1;
    }
    return socketDesc;
}

bool SerialCommunication::send( const char *data, uint16_t length )
{
    if( length > MESSAGE_MAX_SIZE ) {
        std::cerr << "Message size to large"  << std::endl;
    }

    if( mSocketDesc == -1 ) {
        std::cerr << "Not connected"  << std::endl;
        return false;
    }

    //We need to delimit the data
    writeChar( MESSAGE_START );

    for( int i = 0; i < length; ++i ) {
        if( data[i] == MESSAGE_START ) {
            writeChar( MESSAGE_ESCAPE );
            writeChar( MESSAGE_START_ESCAPED );
        } else if( data[i] == MESSAGE_END ) {
            writeChar( MESSAGE_ESCAPE );
            writeChar( MESSAGE_END_ESCAPED );
        } else if( data[i] == MESSAGE_ESCAPE ) {
            writeChar( MESSAGE_ESCAPE );
            writeChar( MESSAGE_ESCAPE_ESCAPED );
        } else {
            writeChar( data[i] );
        }
    }

    writeChar( MESSAGE_END );

    return true;
}

uint16_t SerialCommunication::receive( char *data, uint16_t maxLength )
{
    if( mSocketDesc == -1 ) {
        std::cerr << "Not connected"  << std::endl;
        return -1;
    }

    int readDataBytes = 0;

    bool readingData = false;
    bool readEscape = false;

    //Try to read a complete message
    while( readDataBytes < maxLength ) {
        char readData = readChar();

        switch( readData ) {
        case MESSAGE_START:
            readDataBytes = 0;
            readingData = true;
            readEscape = false;
            break;

        case MESSAGE_ESCAPE:
            if( readingData && !readEscape ) {
                readEscape = true;
            } else {
                readingData = false;
            }
            break;

        case MESSAGE_END:
            if( readingData && !readEscape ) {
                return readDataBytes;
            } else {
                readingData = false;
            }
            break;

        case MESSAGE_START_ESCAPED:
            if( readingData && readEscape ) {
                data[readDataBytes++] = MESSAGE_START;
                readEscape = false;
            } else {
                readingData = false;
            }
            break;

        case MESSAGE_ESCAPE_ESCAPED:
            if( readingData && readEscape ) {
                data[readDataBytes++] = MESSAGE_ESCAPE;
                readEscape = false;
            } else {
                readingData = false;
            }
            break;

        case MESSAGE_END_ESCAPED:
            if( readingData && readEscape ) {
                data[readDataBytes++] = MESSAGE_END;
                readEscape = false;
            } else {
                readingData = false;
            }
            break;


        default:
            if( readingData ) {
                data[readDataBytes++] = readData;
            }
            break;
        }
    }

    return -1;
}

char SerialCommunication::readChar()
{
    char data;
    read( mSocketDesc, &data, 1 );
    return data;
}

void SerialCommunication::writeChar( char data )
{
    write( mSocketDesc, &data, 1 );
}

bool SerialCommunication::setupSocket( int socketDesc, int speed, int parity )
{
    struct termios tty;
    memset( &tty, 0, sizeof tty );
    if( tcgetattr( socketDesc, &tty ) != 0 ) {
        std::cerr << "error from tcgetattr" << std::endl;
        return false;
    }

    cfsetospeed( &tty, speed );
    cfsetispeed( &tty, speed );

    tty.c_cflag = ( tty.c_cflag & ~CSIZE ) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;

    tty.c_iflag &= ~( IXON | IXOFF | IXANY );

    tty.c_cflag |= ( CLOCAL | CREAD );
    tty.c_cflag &= ~( PARENB | PARODD );
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if( tcsetattr( socketDesc, TCSANOW, &tty ) != 0 ) {
        std::cerr << "error from tcsetattr" << std::endl;
        return false;
    }

    return true;
}

}