
/*******************************************************************************
 * Implementation file for UARTs of STM32F750
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_uart.hpp"

#include <hardware/mcu.hpp>

using namespace std;
using namespace hal::device;


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

Stm32f750Uart::Stm32f750Uart(USART_TypeDef* uart,
                             IRQn_Type irq_nb,
                             volatile uint32_t* clk_en_reg,
                             uint32_t clk_en_msk,
                             uint32_t baudrate)
: uart{uart}, irq_nb{irq_nb}, clk_en_reg{clk_en_reg}, clk_en_msk{clk_en_msk}
{
    *clk_en_reg |= clk_en_msk;

    uint16_t usartdiv = apb2_clk_hz / baudrate;
    uart->BRR         = usartdiv;
    /* There's not much we can do in case of overrun error so we'll just disable
     * it.
     * It could be used for debugging though if you're experiencing glitches
     * during UART com. */
    uart->CR3 |= USART_CR3_OVRDIS;

    NVIC_EnableIRQ(irq_nb);
}

Stm32f750Uart::~Stm32f750Uart()
{
    NVIC_DisableIRQ(irq_nb);

    uart->CR1 = 0;
    *clk_en_reg &= ~clk_en_msk;
}

/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

void Stm32f750Uart::onTransmitDataRegisterEmpty()
{
    if (nb_written == nb_to_write) {
        /* We wrote as many characters as were requested and received a TX
         * interrupt => The transfer is finished and we should disable the
         * transmitter and its IRQ then call the driver callback. */
        uart->CR1 &= ~USART_CR1_TE & ~USART_CR1_TXEIE;
        nb_to_write = 0;
        if (write_complete_callback) {
            write_complete_callback(nb_written,
                                    ErrorStatus{ErrorCode::Success});
        }
    } else {
        while ((uart->ISR & USART_ISR_TXE) && (nb_written < nb_to_write)) {
            uart->TDR = buf_out[nb_written];
            nb_written++;
        }
    }
    /* IRQ flag is cleared by the IRQ handler which called this method */
}

void Stm32f750Uart::onReceiveDataRegisterNotEmpty()
{
    do {
        buf_in[nb_read] = uart->RDR;
        nb_read++;

        if ((nb_read == nb_to_read)
            || (buf_in[nb_read - 1] == read_stop_char)) {
            /* We're done reading, disable the emitter, call the callback and
             * return */
            uart->CR1 &= ~USART_CR1_RE & ~USART_CR1_RXNEIE;
            nb_to_read = 0;
            if (read_complete_callback) {
                read_complete_callback(nb_read,
                                       ErrorStatus{ErrorCode::Success});
            }
        }
    } while (uart->ISR & USART_ISR_RXNE);
}

void Stm32f750Uart::startWrite(const char* buf, size_t buf_size)
{
    this->buf_out = buf;
    nb_to_write   = buf_size;
    nb_written    = 0;

    /* Enable USART, Transmitter and TX interrupt */
    uart->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_TXEIE;
}

bool Stm32f750Uart::cancelWrite(size_t& nb_written)
{
    if (nb_to_write == 0) {
        /* Nothing to cancel */
        return false;
    }

    nb_to_write = 0;
    uart->CR1 &= ~USART_CR1_TE & ~USART_CR1_TXEIE;

    nb_written = this->nb_written;
    return true;
}


void Stm32f750Uart::startRead(char* buf,
                              size_t buf_size,
                              std::optional<char> stop_char)
{
    this->buf_in   = buf;
    nb_to_read     = buf_size;
    nb_read        = 0;
    read_stop_char = stop_char;

    /* Enable USART, Emitter and RX interrupt */
    uart->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE;
}

bool Stm32f750Uart::cancelRead(size_t& nb_read)
{
    if (nb_to_read == 0) {
        /* Nothing to cancel */
        return false;
    }

    nb_to_read = 0;
    uart->CR1 &= ~USART_CR1_RE & ~USART_CR1_RXNEIE;

    nb_read = this->nb_read;
    return true;
}
