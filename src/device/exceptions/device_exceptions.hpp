
/*******************************************************************************
 * Header file for exceptions that may be thrown by any device
 ******************************************************************************/

#ifndef _HAL_DEVICE_DEVICE_EXCEPTIONS_HPP
#define _HAL_DEVICE_DEVICE_EXCEPTIONS_HPP

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

struct DeviceException : std::exception {
    const char* what() const noexcept override
    {
        return "Unspecified device exception";
    }
};


struct UnsupportedDeviceOperation : DeviceException {
    const std::string op_name;
    UnsupportedDeviceOperation(std::string&& op_name = ""): op_name{op_name}
    {
    }

    const char* what() const noexcept override
    {
        if (op_name.empty()) {
            return "Unsupported device operation";
        } else {
            return ("Unsupported device operation: " + op_name).c_str();
        }
    }
};


}  // namespace device
}  // namespace hal

#endif
