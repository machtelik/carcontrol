#ifndef __CCM_EVENTLOOP_H__
#define __CCM_EVENTLOOP_H__

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

        int execute();
        void exit();

        void post(std::function<void()> event);

    private:

        volatile bool isRunning = false;

        std::condition_variable eventBarrier;
        std::mutex eventMutex;

        std::queue<std::function<void()> > eventQueue;

    };

} // ccm

#endif /* __CCM_EVENTLOOP_H__ */

