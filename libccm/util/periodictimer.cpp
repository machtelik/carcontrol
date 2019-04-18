#include <utility>

#include "periodictimer.h"

#include <sys/timerfd.h>
#include <thread>
#include <unistd.h>
#include <iostream>

namespace ccm {

    PeriodicTimer::~PeriodicTimer() {
        stop();
    }

    bool PeriodicTimer::start(uint32_t period, std::function<void()> func) {
        if (isRunning) {
            return false;
        }

        timerPeriod = period * 1000;
        periodicFunction = std::move(func);
        timerThread = std::thread(&PeriodicTimer::run, this);

        isRunning = true;

        return true;
    }

    void PeriodicTimer::stop() {
        isRunning = false;
        timerThread.join();
        timerThread.detach();
        periodicFunction = nullptr;
    }

    void PeriodicTimer::run() {
        isRunning = true;
        if (makePeriodic() < 0) {
            return;
        }

        while (isRunning) {
            periodicFunction();
            waitPeriod();
        }

        close(timerDescriptor);
    }

    int PeriodicTimer::makePeriodic() {
        timerDescriptor = timerfd_create(CLOCK_MONOTONIC, 0);
        if (timerDescriptor == -1) {
            return -1;
        }

        uint32_t sec = timerPeriod / 1000000;
        uint32_t ns = (timerPeriod - (sec * 1000000)) * 1000;
        itimerspec timerSpec = {{sec, ns},
                                {sec, ns}};
        return timerfd_settime(timerDescriptor, 0, &timerSpec, nullptr);
    }

    void PeriodicTimer::waitPeriod() {
        uint64_t missed;

        if (read(timerDescriptor, &missed, sizeof(missed)) == -1) {
            std::cerr << "read timer" << std::endl;
            return;
        }
    }

}