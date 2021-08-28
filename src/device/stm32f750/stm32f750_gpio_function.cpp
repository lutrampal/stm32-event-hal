
/*******************************************************************************
 * Implementation file of GPIO config functions for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/gpio_function.hpp>
#include <hardware/mcu.hpp>

using namespace hal;
using namespace device;

/*******************************************************************************
 * STATIC CONSTANT DEFINITIONS
 ******************************************************************************/

constexpr unsigned nb_pins_per_bank = 16;


/*******************************************************************************
 * STATIC FUNCTION DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 * STATIC FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * EXTERN FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

void hal::device::gpioFunctionConfigure(GPIO_TypeDef* bank,
                                        unsigned int pin,
                                        SelFunc sel,
                                        PinSpeed spd,
                                        PullMode pull)
{
    if (pin > nb_pins_per_bank) {
        throw GpioFuncConfigFailure{bank, pin};
    }

    unsigned bank_idx =
        (reinterpret_cast<uintptr_t>(bank) - GPIOA_BASE) / 0x0400U;

    RCC->AHB1ENR |= 0x1UL << (RCC_AHB1ENR_GPIOAEN_Pos + bank_idx);

    bank->OSPEEDR &= ~(0b11 << (pin * 2));
    bank->OSPEEDR |= (static_cast<unsigned>(spd) << (pin * 2));

    bank->PUPDR &= ~(0b11 << (pin * 2));
    bank->PUPDR |= (static_cast<unsigned>(pull) << (pin * 2));

    switch (sel) {
        case SelFunc::Input:
            bank->MODER &= ~(0b11 << (pin * 2));
            bank->MODER |= (0b00 << (pin * 2));
            break;

        case SelFunc::Output:
            bank->MODER &= ~(0b11 << (pin * 2));
            bank->MODER |= (0b01 << (pin * 2));
            break;

        default:
            bank->MODER &= ~(0b11 << (pin * 2));
            bank->MODER |= (0b10 << (pin * 2));

            if (pin <= 7) {
                bank->AFR[0] &= ~(0b1111 << (pin * 4));
                bank->AFR[0] |= (static_cast<unsigned>(sel) << (pin * 4));
            } else {
                bank->AFR[1] &= ~(0b1111 << ((pin - 8) * 4));
                bank->AFR[1] |= (static_cast<unsigned>(sel) << ((pin - 8) * 4));
            }
            break;
    }
}
