
/*******************************************************************************
 * Interface file for UARTs of STM32F750 used in combination with a DMA stream
 * to offload transfers from the CPU.
 * The UART is a character device.
 ******************************************************************************/

#ifndef _HAL_DEVICE_STM32F750_UART_WITH_DMA_HPP
#define _HAL_DEVICE_STM32F750_UART_WITH_DMA_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_dma.hpp"

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

class Stm32f750UartWithDma : public CharacterDevice<char>
{
  public:
    /** Construct a UART object for the STM32F750 MCU. The underlying hardware
     * component will be fully initialized an ready to be used.
     * /!\ This call will not configure GPIOs.
     * In addition to the regular UART, a DMA reference must be provided as well
     * as the channels and streams ID to use for the transfers. */
    Stm32f750UartWithDma(USART_TypeDef* uart,
                         volatile uint32_t* clk_en_reg,
                         uint32_t clk_en_msk,
                         uint32_t baudrate,
                         Stm32f750Dma& dma,
                         unsigned rx_stream_id,
                         unsigned rx_chan_id,
                         unsigned tx_stream_id,
                         unsigned tx_chan_id);
    ~Stm32f750UartWithDma();

    void startWrite(const char* buf, size_t buf_size) override;
    bool cancelWrite() override;

    /** /!\ UART with DMA does not support stop characters, an
     * @ref UnsupportedDeviceOperation will be raised if stop_char is not
     * nullopt. */
    void startRead(char* buf,
                   size_t buf_size,
                   std::optional<char> stop_char = std::nullopt) override;
    bool cancelRead() override;

  private:
    USART_TypeDef* const uart;
    volatile uint32_t* const clk_en_reg;
    const uint32_t clk_en_msk;

    Stm32f750Dma& dma;
    unsigned rx_stream_id;
    unsigned tx_stream_id;

    void dmaTransferCompleted(unsigned stream_id,
                              size_t count,
                              ErrorStatus&& err);
};

}  // namespace device
}  // namespace hal

#endif
