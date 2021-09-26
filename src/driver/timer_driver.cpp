
/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "timer_driver.hpp"

#include "driver_exceptions.hpp"

#include <algorithm>


using namespace std;
using namespace std::placeholders;
using namespace hal::driver;
using namespace hal::device;


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

TimerDriver::Timer::Timer(TimerDriver& owner, Handle handle)
: owner{owner}, handle{handle}
{
}

TimerDriver::TimerDriver(EventLoop& event_loop, device::TimerDevice& device)
: event_loop{event_loop}, device{device}
{
    device.setWaitCompleteCallback(bind(&TimerDriver::completeWait, this, _1));
}


/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

void TimerDriver::completeWait(device::ErrorStatus&& status)
{
    if (wait_queue.front().callback) {
        event_loop.pushEvent(bind(wait_queue.front().callback, status));
    }
    wait_queue.pop_front();

    if (!wait_queue.empty()) {
        device.startWait(wait_queue.front().wait_time.count());
    }
}

void TimerDriver::cancelWait(Handle handle)
{
    /* Disable Timer IRQ: We don't want the timer callback accessing internal
     * class data while we're removing this wait op */
    if (!device.suspendWait()) {
        throw CancelAsyncOpFailure{"Couldn't suspend wait on device"};
    }

    if (wait_queue.empty()) {
        /* Nothing to cancel, maybe the timer already went off and it was the
         * only one? */
        device.resumeWait();
        throw CancelAsyncOpFailure{"Nothing to cancel"};
    }

    if (wait_queue.front().handle == handle) {
        TimerDevice::WaitTimeUnitDuration canceled_wait_time =
            device.getRemainingWaitTime();
        /* This wait operation has already started, ask the device for
         * cancelation */
        if (!device.cancelWait()) {
            /* Too late */
            device.resumeWait();
            throw CancelAsyncOpFailure{"Couldn't cancel wait operation"};
        }
        /* The operation was cancelled and the completion handler won't be
         * called, signal the aborted event to the loop */
        if (wait_queue.front().callback) {
            event_loop.pushEvent(bind(wait_queue.front().callback,
                                      ErrorStatus{ErrorCode::Aborted}));
        }
        wait_queue.pop_front();

        if (!wait_queue.empty()) {
            device.startWait(
                (wait_queue.front().wait_time + canceled_wait_time).count());
        }
    } else {
        /* The wait operation hasn't started, or maybe it was already executed.
         * We'll remove it from the wait queue & add its wait time to the
         * request that was supposed to run after it */
        auto it =
            find_if(++wait_queue.begin(), wait_queue.end(),
                    [handle](const WaitOp& op) { return op.handle == handle; });
        if (it == wait_queue.end()) {
            /* The handle does not exist, the wait operation was already
             * executed */
            device.resumeWait();
            throw CancelAsyncOpFailure{"Wait operation was already exec'd"};
        } else {
            TimerDevice::WaitTimeUnitDuration canceled_wait_time =
                it->wait_time;
            /* The operation was cancelled and the completion handler won't be
             * called, signal the aborted event to the loop */
            if (it->callback) {
                event_loop.pushEvent(
                    bind(it->callback, ErrorStatus{ErrorCode::Aborted}));
            }
            it = wait_queue.erase(it);
            if (it != wait_queue.end()) {
                it->wait_time += canceled_wait_time;
            }
        }
    }

    device.resumeWait();
}


/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

void TimerDriver::Timer::cancelWait()
{
    owner.cancelWait(handle);
}
