
/*******************************************************************************
 * Implementation file for timers of STM32F750
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_uart.hpp"

#include <device/exceptions/character_device_exceptions.hpp>
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

    NVIC_EnableIRQ(irq_nb);
}

Stm32f750Uart::~Stm32f750Uart()
{
    NVIC_DisableIRQ(irq_nb);

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
        while (uart->ISR & USART_ISR_TXE && nb_written < nb_to_write) {
            uart->TDR = buf[nb_written];
            nb_written++;
        }
    }
    /* IRQ flag is cleared by the IRQ handler which called this method */
}

void Stm32f750Uart::startWrite(const char* buf, size_t buf_size)
{
    this->buf   = buf;
    nb_to_write = buf_size;
    nb_written  = 0;

    /* Enable USART, Transmitter and TX interrupt */
    uart->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_TXEIE;
}

bool Stm32f750Uart::suspendWrite()
{
    if (nb_to_write == 0) {
        /* Nothing to suspend */
        return false;
    }

    uart->CR1 &= ~USART_CR1_TE & ~USART_CR1_TXEIE;
    return true;
}

bool Stm32f750Uart::resumeWrite()
{
    if (nb_to_write == 0) {
        /* Nothing to resume */
        return false;
    }

    uart->CR1 |= USART_CR1_TE | USART_CR1_TXEIE;
    return true;
}
