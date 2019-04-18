#include "eventloop.h"

#include <iostream>

namespace ccm {

    int EventLoop::execute() {
        if (isRunning) {
            return EXIT_FAILURE;
        }

        isRunning = true;

        std::function<void()> event;

        while (isRunning) {
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
        isRunning = false;
        eventBarrier.notify_one();
    }

    void EventLoop::post(std::function<void()> event) {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push(event);
        eventBarrier.notify_one();
    }

}