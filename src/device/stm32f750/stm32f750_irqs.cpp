
/*******************************************************************************
 * Implementation file of IRQ handlers for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_irqs.hpp"

#include "stm32f750_timer.hpp"
#include "stm32f750_uart.hpp"

#include <cstdio>
#include <device/system.hpp>
#include <exception>
#include <hardware/mcu.hpp>

using namespace hal;
using namespace hal::device;

extern "C" {
/*******************************************************************************
 * EXTERN VARIABLE DEFINITIONS
 ******************************************************************************/

__attribute__((aligned(0x200))) __attribute__((
    section(".dtcm_vtable"))) volatile InterruptHandler g_vtable[nb_irqs];


/*******************************************************************************
 * STATIC FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

static inline void mHandleTimerEvent(TIM_TypeDef* timer, unsigned id)
{
    if ((timer->SR & TIM_SR_UIF) == TIM_SR_UIF) {
        /* clear interrupt */
        timer->SR &= ~TIM_SR_UIF;
        try {
            static_cast<Stm32f750Timer&>(System::getInstance().getTimer(id))
                .onUpdateInterrupt();
        } catch (const std::exception& e) {
            // TODO: Is there anything better we can do here?
            printf("%s\r\n", e.what());
            handleError();
        }
    }
}

static inline void mHandleUartEvent(USART_TypeDef* uart, unsigned id)
{
    try {
        uint32_t cr1 = uart->CR1;
        uint32_t isr = uart->ISR;
        if ((cr1 & USART_CR1_TXEIE) && (isr & USART_ISR_TXE)) {
            /* Transmit data registry empty and we want to send data.
             * TXE bit will be cleared when writing the next char to the TDR
             * register in the following call. */
            static_cast<Stm32f750Uart&>(System::getInstance().getUart(id))
                .onTransmitDataRegisterEmpty();
        }
        if ((cr1 & USART_CR1_RXNEIE) && (isr & USART_ISR_RXNE)) {
            /* Receive data registry not empty and we're expecting data.
             * RXNE will be cleared when reading the next char from the RDR
             * register in the following call. */
            static_cast<Stm32f750Uart&>(System::getInstance().getUart(id))
                .onReceiveDataRegisterNotEmpty();
        }
    } catch (const std::exception& e) {
        // TODO: Is there anything better we can do here?
        printf("%s\r\n", e.what());
        handleError();
    }
}


/*******************************************************************************
 * EXTERN FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

void handleError(void)
{
    while (1) {};
}

void handleTIM2Event(void)
{
    mHandleTimerEvent(TIM2, 2);
}

void handleTIM5Event(void)
{
    mHandleTimerEvent(TIM5, 5);
}

void handleUSART1Event(void)
{
    mHandleUartEvent(USART1, 1);
}
}
