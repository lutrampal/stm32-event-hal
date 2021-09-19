
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
#include <string>

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
        return (std::string{"Invalid timer ID: "} + std::to_string(id)).c_str();
    }
};

struct InvalidUartIdException : SystemException {
    unsigned id;

    InvalidUartIdException(unsigned id): id{id}
    {
    }

    const char* what() const noexcept override
    {
        return (std::string{"Invalid UART ID: "} + std::to_string(id)).c_str();
    }
};

struct InvalidDmaIdException : SystemException {
    unsigned id;

    InvalidDmaIdException(unsigned id): id{id}
    {
    }

    const char* what() const noexcept override
    {
        return (std::string{"Invalid DMA ID: "} + std::to_string(id)).c_str();
    }
};

struct UnimplementedDeviceException : SystemException {
    std::string device_name;

    UnimplementedDeviceException(std::string&& device_name)
    : device_name{device_name}
    {
    }

    const char* what()
    {
        return (std::string{"Unimplemented hardware device: "} + device_name)
            .c_str();
    }
};

}  // namespace device
}  // namespace hal

#endif
