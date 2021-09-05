
/*******************************************************************************
 * Implementation file for timer driver templated functions
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "driver_exceptions.hpp"
#include "timer_driver.hpp"


/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

template<typename TRep, typename TPeriod>
hal::driver::TimerDriver::Timer hal::driver::TimerDriver::asyncWait(
    const std::chrono::duration<TRep, TPeriod>& wait_time,
    const std::function<void(hal::device::ErrorStatus&)>&& event_callback)
{
    using namespace std;
    using namespace hal::device;

    Handle handle = next_handle++;
    Timer timer{*this, handle};

    /* Disable Timer IRQ: We don't want the timer callback accessing internal
     * class data while we're adding this wait op */
    if (!device.suspendWait()) {
        throw StartAsyncOpFailure{};
    }

    TimerDevice::WaitTimeUnitDuration dev_wait_duration =
        chrono::duration_cast<TimerDevice::WaitTimeUnitDuration>(wait_time);

    if (wait_queue.empty()) {
        device.startWait(dev_wait_duration.count());
        wait_queue.push_front(
            WaitOp{handle, dev_wait_duration, event_callback});
        return timer;
    }

    /* Another wait operation is running, we need to schedule the new one for
     * later */
    TimerDevice::WaitTimeUnitDuration remaining_wait_time =
        device.getRemainingWaitTime();

    if (remaining_wait_time > dev_wait_duration) {
        /* The running wait is longer than what we need, cancel it and
         * reschedule it for later */
        if (!device.cancelWait()) {
            /* We cannot go further */
            device.resumeWait();
            throw StartAsyncOpFailure{};
        }

        device.startWait(dev_wait_duration.count());
        wait_queue.front().wait_time = remaining_wait_time - dev_wait_duration;
        wait_queue.push_front(
            WaitOp{handle, dev_wait_duration, event_callback});
    } else {
        /* The new wait op won't be started immediately. We need to
         * schedule it:
         * - Substract from this request's duration the duration of all wait
         *      operations that will be executed before this one
         * - Insert the operation in the wait queue
         * - Substract the remaining time to other operations that will be
         * executed after this one */
        auto it = wait_queue.begin();
        while (dev_wait_duration > it->wait_time && it != wait_queue.end()) {
            dev_wait_duration -= it->wait_time;
            ++it;
        }
        wait_queue.insert(it,
                          WaitOp{handle, dev_wait_duration, event_callback});
        if (it != wait_queue.end()) {
            it->wait_time -= dev_wait_duration;
        }
    }

    device.resumeWait();

    return timer;
}
