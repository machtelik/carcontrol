#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <stdint.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>

namespace ccm
{

class EventLoop
{

public:

    EventLoop();
    virtual ~EventLoop();

    virtual int execute();
    virtual void exit();

    void post( std::function<void()> event );

private:

    volatile bool mRunning;

    std::condition_variable mEventBarrier;
    std::mutex mEventMutex;

    std::queue< std::function<void()> > mEventQueue;

};

} // ccm

#endif /* __EVENTLOOP_H__ */

