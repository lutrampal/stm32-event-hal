
/*******************************************************************************
 * A simple class used to configure GPIOs for use by peripherals
 ******************************************************************************/

#ifndef _HAL_DEVICE_GPIO_FUNCTION_HPP
#define _HAL_DEVICE_GPIO_FUNCTION_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <cstddef>
#include <exception>
#include <hardware/mcu.hpp>


namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

enum class SelFunc {
    Alt0 = 0b0000,
    Alt1,
    Alt2,
    Alt3,
    Alt4,
    Alt5,
    Alt6,
    Alt7,
    Alt8,
    Alt9,
    Alt10,
    Alt11,
    Alt12,
    Alt13,
    Alt14,
    Alt15,
    Input,
    Output
};

enum class PinSpeed { Low, Medium, High, VeryHigh };

enum class PullMode { NoPull, PullUp, PullDown };

struct GpioFuncConfigFailure : std::exception {
    GPIO_TypeDef* bank;
    unsigned int index;

    GpioFuncConfigFailure(GPIO_TypeDef* bank, unsigned int index)
    : bank{bank}, index{index}
    {
    }

    const char* what()
    {
        return "Failed to configure GPIO pin";
    }
};


/*******************************************************************************
 * EXTERNAL FUNCTION DECLARATIONS
 ******************************************************************************/

void gpioFunctionConfigure(GPIO_TypeDef* bank,
                           unsigned int pin,
                           SelFunc sel,
                           PinSpeed spd,
                           PullMode pull = PullMode::NoPull);


}  // namespace device
}  // namespace hal

#endif
