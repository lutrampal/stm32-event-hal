
/*******************************************************************************
 * Interface file for UARTs of STM32F750
 * The UART is a character device
 ******************************************************************************/

#ifndef _HAL_DEVICE_STM32F750_UART_HPP
#define _HAL_DEVICE_STM32F750_UART_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <cstdint>
#include <device/character_device.hpp>
#include <hardware/mcu.hpp>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class Stm32f750Uart : public CharacterDevice<char>
{
  public:
    /** Construct a UART object for the STM32F750 MCU. The underlying hardware
     * component will be fully initialized an ready to be used.
     * /!\ This call will not configure GPIOs. */
    Stm32f750Uart(USART_TypeDef* uart,
                  IRQn_Type irq_nb,
                  volatile uint32_t* clk_en_reg,
                  uint32_t clk_en_msk,
                  uint32_t baudrate);
    ~Stm32f750Uart();

    /** This method should be called in the interrupt handler to signal to the
     * device that new data may be transmitted. */
    void onTransmitDataRegisterEmpty();

    void startWrite(const char* buf, size_t buf_size) override;
    bool suspendWrite() override;
    bool resumeWrite() override;

  private:
    USART_TypeDef* const uart;
    const IRQn_Type irq_nb;
    volatile uint32_t* const clk_en_reg;
    const uint32_t clk_en_msk;
};

}  // namespace device
}  // namespace hal

#endif
