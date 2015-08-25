#ifndef __SERIALCOMMUNICATION_H__
#define __SERIALCOMMUNICATION_H__

#include <netinet/in.h>
#include <string>

#include "communication/communication.h"

namespace ccm
{

class SerialCommunication : public Communication
{

public:
    static const uint8_t TYPE = 1;

    SerialCommunication( const std::string &device, int baudRate );
    virtual ~SerialCommunication();

    bool connect();
    bool disconnect();

    bool send( const char *data, uint16_t length );
    uint16_t receive( char* data, uint16_t maxLength );

protected:

    enum ReceiveState {WAIT_FOR_START = 0, READING_CHAR = 1, READING_ESCAPE = 2};
    
    static int createSocket( const std::string &device );
    static bool setupSocket( int socketDesc, int speed, int parity );

    char readChar();
    void writeChar( char data );

private:
    int mSocketDesc;
    std::string mDevice;
    int mBaudRate;

};

} // ccm

#endif /* __SERIALCOMMUNICATION_H__ */


