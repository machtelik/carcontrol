#ifndef __CCM_PERIODICTIMER_H__
#define __CCM_PERIODICTIMER_H__

#include <thread>
#include <stdint.h>
#include <functional>
#include <memory>

namespace ccm {

    class PeriodicTimer {

    public:
        explicit PeriodicTimer() = default;
        virtual ~PeriodicTimer();

        bool start(uint32_t period, std::function<void()> func);
        void stop();

    protected:

        int makePeriodic();
        void waitPeriod();

        void run();

    private:
        std::function<void()> periodicFunction;

        volatile bool isRunning = false;
        uint32_t timerPeriod = 1;

        int timerDescriptor = -1;

        std::unique_ptr<std::thread> timerThread;
    };

} // ccm

#endif /* __CCM_PERIODICTIMER_H__ */

