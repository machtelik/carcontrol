#ifndef __PERIODICTIMER_H__
#define __PERIODICTIMER_H__

#include <thread>
#include <stdint.h>

namespace ccm {
  
class PeriodicTimer {
  
public:
  PeriodicTimer(uint32_t period, std::function< void() > func);
  virtual ~PeriodicTimer();
  
protected:
 
    int makePeriodic();
    void waitPeriod();
    
    void run();
      
private:
    std::function<void()> mFunc;
    
    volatile bool mRunning;
    uint32_t mPeriod;
    
    int timer_fd;
    uint64_t wakeups_missed;
    
    std::thread *mThread;
    
  
};

} // ccm

#endif /* __PERIODICTIMER_H__ */
 
