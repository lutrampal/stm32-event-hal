
/*******************************************************************************
 * Implementation file of IRQ handlers for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_irqs.hpp"

#include "stm32f750_dma.hpp"
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
 * STATIC VARIABLE DEFINITIONS
 ******************************************************************************/

static System& sys = System::getInstance();


/*******************************************************************************
 * STATIC FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

static inline void mHandleTimerEvent(TIM_TypeDef* timer, unsigned id)
{
    try {
        Stm32f750Timer& timer_dev =
            static_cast<Stm32f750Timer&>(sys.getTimer(id));

        if ((timer->SR & TIM_SR_UIF) == TIM_SR_UIF) {
            /* clear interrupt */
            timer->SR &= ~TIM_SR_UIF;
            try {
                timer_dev.onUpdateInterrupt();
            } catch (const std::exception& e) {
                // TODO: Is there anything better we can do here?
                printf("%s\r\n", e.what());
                handleError();
            }
        }
    } catch (const std::exception& e) {
        // TODO: Is there anything better we can do here?
        printf("%s\r\n", e.what());
        handleError();
    }
}

static inline void mHandleUartEvent(USART_TypeDef* uart, unsigned id)
{
    try {
        Stm32f750Uart& uart_dev = static_cast<Stm32f750Uart&>(sys.getUart(id));
        uint32_t cr1            = uart->CR1;
        uint32_t isr            = uart->ISR;
        if ((cr1 & USART_CR1_TXEIE) && (isr & USART_ISR_TXE)) {
            /* Transmit data registry empty and we want to send data.
             * TXE bit will be cleared when writing the next char to the TDR
             * register in the following call. */
            uart_dev.onTransmitDataRegisterEmpty();
        }
        if ((cr1 & USART_CR1_RXNEIE) && (isr & USART_ISR_RXNE)) {
            /* Receive data registry not empty and we're expecting data.
             * RXNE will be cleared when reading the next char from the RDR
             * register in the following call. */
            uart_dev.onReceiveDataRegisterNotEmpty();
        }
    } catch (const std::exception& e) {
        // TODO: Is there anything better we can do here?
        printf("%s\r\n", e.what());
        handleError();
    }
}

/* Here the various flag bitmasks are given as parameters because they are
 * calculated using constexpr and the stream_id is known at compile time. */
static inline void mHandleDmaEvent(volatile uint32_t* isr,
                                   volatile uint32_t* ifcr,
                                   unsigned dma_id,
                                   unsigned stream_id,
                                   uint32_t TCIFx,
                                   uint32_t CTCIFx,
                                   uint32_t TEIFx,
                                   uint32_t CTEIFx,
                                   uint32_t DMEIFx,
                                   uint32_t CDMEIFx,
                                   uint32_t FEIFx,
                                   uint32_t CFEIFx)
{
    try {
        Stm32f750Dma& dma_dev = static_cast<Stm32f750Dma&>(sys.getDma(dma_id));

        if (*isr & TCIFx) {
            /* Transfer complete interrupt */
            *ifcr |= CTCIFx;
            dma_dev.onTransferComplete(stream_id);
        }

        if (*isr & TEIFx) {
            /* Transfer error */
            *ifcr |= CTEIFx;
            dma_dev.onTransferError(stream_id);
        }

        if (*isr & DMEIFx) {
            /* Direct mode error */
            *ifcr |= CDMEIFx;
            dma_dev.onDirectModeError(stream_id);
        }

        if (*isr & FEIFx) {
            /* FIFO error */
            *ifcr |= CFEIFx;
            dma_dev.onFifoError(stream_id);
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

void handleDMA2Stream7Event(void)
{
    mHandleDmaEvent(&DMA2->HISR, &DMA2->HIFCR, 2, 7, Stm32f750Dma::TCIFx(7),
                    Stm32f750Dma::CTCIFx(7), Stm32f750Dma::TEIFx(7),
                    Stm32f750Dma::CTEIFx(7), Stm32f750Dma::DMEIFx(7),
                    Stm32f750Dma::CDMEIFx(7), Stm32f750Dma::FEIFx(7),
                    Stm32f750Dma::CFEIFx(7));
}
}
