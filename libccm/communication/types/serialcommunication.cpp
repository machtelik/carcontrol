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

    if( write( mSocketDesc, data, length ) != length ) {
        return false;
    }

    return true;
}

u_int16_t SerialCommunication::receive( char *data, uint16_t maxLength )
{
    if( mSocketDesc == -1 ) {
        std::cerr << "Not connected"  << std::endl;
        return -1;
    }

    uint16_t readHeaderSize = read( mSocketDesc, data, MESSAGE_HEADER_SIZE );
    if( readHeaderSize != MESSAGE_HEADER_SIZE ) {
        std::cerr << "Could not read the header"  << std::endl;
        return -1;
    }

    uint16_t payloadSize = ( ( data[1] << 8 ) + data[0] ) - readHeaderSize;

    if( readHeaderSize + payloadSize > maxLength ) {
        std::cerr << "Packet too large"  << std::endl;
        return -1;
    }

    uint16_t readPayloadSize = read( mSocketDesc, &data[readHeaderSize], payloadSize );

    if( readPayloadSize != payloadSize ) {
        std::cerr << "Cold not read payload"  << std::endl;
    }

    return readHeaderSize + readPayloadSize;
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