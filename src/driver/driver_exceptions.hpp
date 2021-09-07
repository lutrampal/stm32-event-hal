
/*******************************************************************************
 * Header file for exceptions that may be thrown by any driver
 ******************************************************************************/

#ifndef _HAL_DRIVER_DRIVER_EXCEPTIONS_HPP
#define _HAL_DRIVER_DRIVER_EXCEPTIONS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <exception>
#include <string>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITIONS
 ******************************************************************************/

struct StartAsyncOpFailure : std::exception {
    const std::string reason;
    StartAsyncOpFailure(std::string&& reason = ""): reason{reason}
    {
    }

    const char* what() const noexcept override
    {
        if (reason.empty()) {
            return "Failed to start asynchronous operation";
        } else {
            return ("Failed to start asynchronous operation: " + reason)
                .c_str();
        }
    }
};

struct CancelAsyncOpFailure : std::exception {
    const std::string reason;
    CancelAsyncOpFailure(std::string&& reason = ""): reason{reason}
    {
    }

    const char* what() const noexcept override
    {
        if (reason.empty()) {
            return "Failed to cancel asynchronous operation";
        } else {
            return ("Failed to cancel asynchronous operation: " + reason)
                .c_str();
        }
    }
};


}  // namespace device
}  // namespace hal

#endif
