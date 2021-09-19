
/*******************************************************************************
 * Header file for exceptions that can be returned by the TimerDevice class
 ******************************************************************************/

#ifndef _HAL_DEVICE_TIMER_EXCEPTIONS_HPP
#define _HAL_DEVICE_TIMER_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "device_exceptions.hpp"

#include <device/timer_device.hpp>
#include <exception>
#include <hardware/mcu.hpp>
#include <string>

namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

struct TimerException : DeviceException {
    virtual const char* what() const noexcept override
    {
        return "Unspecified Timer exception";
    }

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
        return ("Invalid timer count: " + std::to_string(count)
                + " while max count is " + std::to_string(max_count))
            .c_str();
    }
};

}  // namespace device
}  // namespace hal

#endif
