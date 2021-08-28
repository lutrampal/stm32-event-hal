
/*******************************************************************************
 * Header file for exceptions that can be returned by the TimerDevice class
 ******************************************************************************/

#ifndef _HAL_DEVICE_TIMER_EXCEPTIONS_HPP
#define _HAL_DEVICE_TIMER_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/timer_device.hpp>
#include <exception>
#include <hardware/mcu.hpp>

namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

struct TimerException : std::exception {
    virtual const char* what() const noexcept override
    {
        return "Unspecified Timer exception";
    }

  protected:
    TimerException()
    {
    }
};

struct InvalidTimerCountException : TimerException {
    TimerDevice::WaitTimeUnitDuration::rep count;
    TimerDevice::WaitTimeUnitDuration::rep max_count;

    InvalidTimerCountException(TimerDevice::WaitTimeUnitDuration::rep count,
                               TimerDevice::WaitTimeUnitDuration::rep max_count)
    : count{count}, max_count{max_count}
    {
    }

    const char* what() const noexcept override
    {
        return "Invalid timer count";
    }
};

}  // namespace device
}  // namespace hal

#endif
