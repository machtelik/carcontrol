#ifndef __APP_H__
#define __APP_H__

#include <component.h>

class App : public ccm::Component {

public:

    App(int argc, char **argv);

protected:

    bool begin() override;
    void loop() override;

    bool onMessageReceived(ccm::Message *message) override;

private:

};

#endif /* __APP_H__ */

