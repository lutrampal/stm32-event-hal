
/*******************************************************************************
 * Header file for exceptions that can be returned by the DmaDevice class
 ******************************************************************************/

#ifndef _HAL_DEVICE_DMA_EXCEPTIONS_HPP
#define _HAL_DEVICE_DMA_EXCEPTIONS_HPP

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

struct DmaException : DeviceException {
    virtual const char* what() const noexcept override
    {
        return "Unspecified DMA exception";
    }

    DmaException()
    {
    }
};

struct InvalidStreamIdException : DmaException {
    unsigned stream_id;

    InvalidStreamIdException(unsigned stream_id): stream_id{stream_id}
    {
    }

    const char* what() const noexcept override
    {
        return ("Invalid stream ID: " + std::to_string(stream_id)).c_str();
    }
};

struct InvalidTransferSizeException : DmaException {
    DmaDevice::DataWidth data_width;
    size_t count;

    InvalidTransferSizeException(DmaDevice::DataWidth data_width, size_t count)
    : data_width{data_width}, count{count}
    {
    }

    const char* what() const noexcept override
    {
        return ("Invalid Transfer Size: count=" + std::to_string(count)
                + " data_width="
                + std::to_string(static_cast<unsigned>(data_width)))
            .c_str();
    }
};

}  // namespace device
}  // namespace hal

#endif
