
/*******************************************************************************
 * Interface file for timer devices
 ******************************************************************************/

#ifndef _HAL_DEVICE_TIMER_DEVICE_HPP
#define _HAL_DEVICE_TIMER_DEVICE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "error_status.hpp"

#include <chrono>
#include <functional>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class TimerDevice
{
  public:
    typedef std::chrono::duration<uint32_t, std::micro> WaitTimeUnitDuration;

    /** Set the callback function that will be called once the device completes
     * a wait operation.
     * @param callback
     *  The new callback function. It will receive as paramater an error status
     * indicating if the wait operation was succesfully executed or not. */
    void setWaitCompleteCallback(std::function<void(ErrorStatus&&)> callback);

    virtual WaitTimeUnitDuration getRemainingWaitTime()     = 0;
    virtual bool suspendWait()                              = 0;
    virtual bool resumeWait()                               = 0;
    virtual bool cancelWait()                               = 0;
    virtual bool startWait(WaitTimeUnitDuration::rep count) = 0;
    virtual bool completeWait()                             = 0;
    /* An active wait that is sometimes handy when initializing peripherals and
     * such. No check is performed to see whether or not a Wait operation is
     * currently running so this should only be used during init, before
     * entering the event loop. */
    virtual void usleep(WaitTimeUnitDuration::rep count) = 0;

  protected:
    WaitTimeUnitDuration::rep programmed_count;
    std::function<void(ErrorStatus&&)> callback;
};

}  // namespace device
}  // namespace hal

#endif
