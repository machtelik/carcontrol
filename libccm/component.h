#ifndef __CCM_COMPONENT_H__
#define __CCM_COMPONENT_H__

#include <memory>

#include "eventloop/eventloop.h"
#include "communication/message/message.h"
#include "communication/communication.h"

namespace ccm {

    class PeriodicTimer;

    class Component : public EventLoop {

    public:
        Component(int argc, char **argv);

        ~Component() override;

        int execute() override;
        void exit() override;

        void sendMessage(Message *message);

    protected:

        virtual bool begin() = 0;
        virtual void loop() = 0;

        virtual bool onMessageReceived(Message *message) = 0;

    private:

        std::unique_ptr<PeriodicTimer> periodicTimer;
        std::unique_ptr<Communication> communication;

        uint32_t loopInterval;

    };

} // ccm

#endif /* __CCM_COMPONENT_H__ */

