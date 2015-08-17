#include "periodictimer.h"

#include <sys/timerfd.h>
#include <thread>
#include <unistd.h>
#include <iostream>

namespace ccm
{

PeriodicTimer::PeriodicTimer( uint32_t period, std::function<void()> func ) :
    mFunc( func ),
    mRunning( true ),
    mPeriod( period * 1000 )
{
    mThread = new std::thread( &PeriodicTimer::run, this );
}


PeriodicTimer::~PeriodicTimer()
{
    mRunning = false;
    mThread->join();
}


void PeriodicTimer::run()
{
    mRunning = true;
    if( makePeriodic() < 0 ) {
        return;
    }

    while( mRunning ) {
        mFunc();
        waitPeriod();
    }

    close( timer_fd );
}

int PeriodicTimer::makePeriodic()
{
    struct itimerspec itval;

    timer_fd = timerfd_create( CLOCK_MONOTONIC, 0 );
    wakeups_missed = 0;
    if( timer_fd == -1 ) {
        return -1;
    }

    uint32_t sec = mPeriod / 1000000;
    uint32_t ns = ( mPeriod - ( sec * 1000000 ) ) * 1000;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = ns;
    itval.it_value.tv_sec = sec;
    itval.it_value.tv_nsec = ns;
    return timerfd_settime( timer_fd, 0, &itval, NULL );
}

void PeriodicTimer::waitPeriod()
{
    uint64_t missed;

    int ret = read( timer_fd, &missed, sizeof( missed ) );
    if( ret == -1 ) {
        std::cerr << "read timer" << std::endl;
        return;
    }

    if( missed > 0 ) {
        wakeups_missed += ( missed - 1 );
    }
}

}