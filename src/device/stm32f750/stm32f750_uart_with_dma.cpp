
/*******************************************************************************
 * Implementation file for UARTs of STM32F750 used in conjunction with DMAs
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_uart_with_dma.hpp"

#include <hardware/mcu.hpp>

using namespace std;
using namespace hal::device;


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

Stm32f750UartWithDma::Stm32f750UartWithDma(USART_TypeDef* uart,
                                           volatile uint32_t* clk_en_reg,
                                           uint32_t clk_en_msk,
                                           uint32_t baudrate,
                                           Stm32f750Dma& dma,
                                           unsigned rx_stream_id,
                                           unsigned rx_chan_id,
                                           unsigned tx_stream_id,
                                           unsigned tx_chan_id)
: uart{uart}, clk_en_reg{clk_en_reg}, clk_en_msk{clk_en_msk}, dma{dma},
  rx_stream_id{rx_stream_id}, tx_stream_id{tx_stream_id}
{
    *clk_en_reg |= clk_en_msk;

    uint16_t usartdiv = apb2_clk_hz / baudrate;
    uart->BRR         = usartdiv;
    /* There's not much we can do in case of overrun error so we'll just disable
     * it.
     * It could be used for debugging though if you're experiencing glitches
     * during UART com. */
    uart->CR3 |= USART_CR3_OVRDIS;

    dma.setChannel(rx_stream_id, rx_chan_id);
    dma.setChannel(tx_stream_id, tx_chan_id);
    dma.setTransferCompleteCallback(
        bind(&Stm32f750UartWithDma::dmaTransferCompleted, this,
             placeholders::_1, placeholders::_2, placeholders::_3));
}

Stm32f750UartWithDma::~Stm32f750UartWithDma()
{
    uart->CR1 = 0;
    *clk_en_reg &= ~clk_en_msk;
}

/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/


void Stm32f750UartWithDma::dmaTransferCompleted(unsigned stream_id,
                                                size_t count,
                                                ErrorStatus&& err)
{
    if (stream_id == tx_stream_id) {
        uart->CR1 &= ~USART_CR1_TE;
        uart->CR3 &= ~USART_CR3_DMAT;
        write_complete_callback(count, move(err));
    } else if (stream_id == rx_stream_id) {
        uart->CR1 &= ~USART_CR1_RE;
        uart->CR3 &= ~USART_CR3_DMAR;
        read_complete_callback(count, move(err));
    }
}

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

void Stm32f750UartWithDma::startWrite(const char* buf, size_t buf_size)
{
    DmaDevice::Location src = {reinterpret_cast<uintptr_t>(buf),
                               DmaDevice::DataWidth::Byte, true};
    DmaDevice::Location dst = {reinterpret_cast<uintptr_t>(&uart->TDR),
                               DmaDevice::DataWidth::Byte, false};

    uart->ICR |= USART_ICR_TCCF;

    dma.startTransfer(tx_stream_id, src, dst, buf_size,
                      DmaDevice::TransferDirection::MemToPeriph,
                      DmaDevice::TransferPriority::VeryHigh);

    uart->CR1 |= USART_CR1_UE;
    uart->CR3 |= USART_CR3_DMAT;
    uart->CR1 |= USART_CR1_TE;
}

bool Stm32f750UartWithDma::cancelWrite(size_t& nb_written)
{
    nb_written = 0;
    // TODO
    return false;
}


void Stm32f750UartWithDma::startRead(char* buf,
                                     size_t buf_size,
                                     std::optional<char> stop_char)
{
    // TODO
}

bool Stm32f750UartWithDma::cancelRead(size_t& nb_read)
{
    nb_read = 0;
    // TODO
    return false;
}
