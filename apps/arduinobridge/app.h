#ifndef __APP_H__
#define __APP_H__

#include "component.h"
#include <memory>
#include "communication/types/serialcommunication.h"

class App : public ccm::Component {

public:

    App(int argc, char **argv);

protected:

    bool begin() override;
    void loop() override;

    bool onMessageReceived(ccm::Message *message) override;

private:

    std::unique_ptr<ccm::Communication> serialCommunication;

};

#endif /* __APP_H__ */

