
/*******************************************************************************
 * A generic timer driver that can be used with timer devices that implement the
 * correct interface.
 ******************************************************************************/

#ifndef _HAL_DRIVER_TIMER_DRIVER_HPP
#define _HAL_DRIVER_TIMER_DRIVER_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <chrono>
#include <device/timer_device.hpp>
#include <event_loop.hpp>

namespace hal
{
namespace driver
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class TimerDriver
{
  public:
    TimerDriver(EventLoop& event_loop, device::TimerDevice& device);

    typedef unsigned Handle;

    class Timer
    {
      public:
        Timer(TimerDriver& owner, Handle handle);

        void cancelWait();

      private:
        TimerDriver& owner;
        Handle handle;
    };

    /** Start an asynchronous wait operation
     * @param timeout
     *  The wait time
     * @param event_callback
     *  The event that will be pushed to the event loop once the wait time is
     * finished. This callback will receive an error status as parameter,
     * indicating if the wait operation succeeded or not. */
    template<typename TRep, typename TPeriod>
    Timer asyncWait(
        const std::chrono::duration<TRep, TPeriod>& timeout,
        const std::function<void(device::ErrorStatus&)>& event_callback);

  private:
    struct WaitOp {
        Handle handle;
        device::TimerDevice::WaitTimeUnitDuration wait_time;
        std::function<void(device::ErrorStatus&)> callback;
    };

    EventLoop& event_loop;
    device::TimerDevice& device;
    std::list<WaitOp> wait_queue;
    Handle next_handle;

    friend Timer;

    void completeWait(device::ErrorStatus&& status);
    void cancelWait(Handle handle);
};

}  // namespace driver
}  // namespace hal

#include "timer_driver_impl.hpp"

#endif
