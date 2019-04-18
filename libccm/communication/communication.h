#ifndef __CCM_COMMUNICATION_H__
#define __CCM_COMMUNICATION_H__

#include <stdint.h>
#include <functional>
#include <memory>
#include <thread>

namespace ccm {

    class Message;

    class EventLoop;

    class Communication {

    public:

        explicit Communication();
        virtual ~Communication() = default;

        bool start();
        void stop();

        bool isConnected();

        void send(Message *message);

        void setMessageCallback(std::function<void(const Message *)> callback);

    protected:
        virtual bool connect() = 0;
        virtual bool disconnect() = 0;

        virtual bool sendMessage(const Message *message) = 0;
        virtual void receiveMessages() = 0;

        void messageReceived(Message *message);

    private:

        volatile bool connected = false;

        std::unique_ptr<EventLoop> eventLoop;
        std::unique_ptr<std::thread> eventThread;
        std::unique_ptr<std::thread> receiveThread;

        std::function<void(const Message *)> messageCallback;

    };

} // ccm

#endif /* __CCM_COMMUNICATION_H__ */


