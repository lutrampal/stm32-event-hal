
/*******************************************************************************
 * Header file for exceptions that can be returned only by the System class
 ******************************************************************************/

#ifndef _HAL_DEVICE_SYSTEM_EXCEPTIONS_HPP
#define _HAL_DEVICE_SYSTEM_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <exception>
#include <hardware/mcu.hpp>

namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

struct SystemException : std::exception {
    virtual const char* what() const noexcept override
    {
        return "Unspecified system exception";
    }

  protected:
    SystemException()
    {
    }
};

struct InvalidTimerIdException : SystemException {
    unsigned id;

    InvalidTimerIdException(unsigned id): id{id}
    {
    }

    const char* what() const noexcept override
    {
        return "Invalid timer ID";
    }
};

}  // namespace device
}  // namespace hal

#endif
