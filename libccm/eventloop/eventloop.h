#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <stdint.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace ccm {

    class EventLoop {

    public:

        explicit EventLoop() = default;
        virtual ~EventLoop() = default;

        virtual int execute();
        virtual void exit();

        virtual void post(std::function<void()> event);

    private:

        volatile bool isRunning = false;

        std::condition_variable eventBarrier;
        std::mutex eventMutex;

        std::queue<std::function<void()> > eventQueue;

    };

} // ccm

#endif /* __EVENTLOOP_H__ */

