
/*******************************************************************************
 * Header file for exceptions that can be returned by the CharacterDevice class
 ******************************************************************************/

#ifndef _HAL_DEVICE_CHARACTER_DEVICE_EXCEPTIONS_HPP
#define _HAL_DEVICE_CHARACTER_DEVICE_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/character_device.hpp>
#include <exception>
#include <hardware/mcu.hpp>

namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

struct CharacterDeviceException : std::exception {
    virtual const char* what() const noexcept override
    {
        return "Unspecified character device exception";
    }

  protected:
    CharacterDeviceException()
    {
    }
};

struct CharWriteRejectedException : CharacterDeviceException {
    const char* what() const noexcept override
    {
        return "The requested character could not be written";
    }
};

}  // namespace device
}  // namespace hal

#endif
