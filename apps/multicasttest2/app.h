#ifndef __APP_H__
#define __APP_H__

#include <component.h>

class App : public ccm::Component
{

public:

    App( int argc, char **argv );

protected:

    virtual bool begin();
    virtual bool loop();
    virtual bool messageReceived( const ccm::Message *message );

private:

};

#endif /* __APP_H__ */

