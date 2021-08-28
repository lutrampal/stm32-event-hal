
/*******************************************************************************
 * Header file for exceptions that may be thrown by any driver
 ******************************************************************************/

#ifndef _HAL_DRIVER_DRIVER_EXCEPTIONS_HPP
#define _HAL_DRIVER_DRIVER_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <exception>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITIONS
 ******************************************************************************/

class StartAsyncOpFailure : std::exception
{
    const char* what() const noexcept override
    {
        return "Failed to start asynchronous operation";
    }
};

class CancelAsyncOpFailure : std::exception
{
    const char* what() const noexcept override
    {
        return "Failed to cancel asynchronous operation";
    }
};


}  // namespace device
}  // namespace hal

#endif
