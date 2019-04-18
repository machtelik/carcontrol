#include "eventloop.h"

#include <iostream>

namespace ccm {

    int EventLoop::execute() {
        if (isExecuting) {
            return EXIT_FAILURE;
        }

        isExecuting = true;

        std::function<void()> event;

        while (isExecuting) {
            {
                std::unique_lock<std::mutex> lock(eventMutex);
                eventBarrier.wait(lock, [&]() {
                    return !eventQueue.empty();
                });

                event = eventQueue.front();
                eventQueue.pop();
            }

            event();
        }

        return EXIT_SUCCESS;

    }

    void EventLoop::exit() {
        isExecuting = false;
        eventBarrier.notify_one();
    }

    void EventLoop::post(std::function<void()> event) {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push(event);
        eventBarrier.notify_one();
    }

    bool EventLoop::isRunning() {
        return isExecuting;
    }

}