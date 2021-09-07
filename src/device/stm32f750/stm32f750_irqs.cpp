
/*******************************************************************************
 * Implementation file of IRQ handlers for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_irqs.hpp"

#include "stm32f750_timer.hpp"
#include "stm32f750_uart.hpp"

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

static inline void handle_timer_event(TIM_TypeDef* timer, unsigned id)
{
    if ((timer->SR & TIM_SR_UIF) == TIM_SR_UIF) {
        /* clear interrupt */
        timer->SR &= ~TIM_SR_UIF;
        try {
            static_cast<Stm32f750Timer&>(System::getInstance().getTimer(id))
                .onUpdateInterrupt();
        } catch (const std::exception& e) {
            // TODO: Is there anything better we can do here?
            handleError();
        }
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
    handle_timer_event(TIM2, 2);
}

void handleTIM5Event(void)
{
    handle_timer_event(TIM5, 5);
}

void handleUSART1Event(void)
{
    if (USART1->ISR & USART_ISR_TXE) {
        /* Transmit data registry empty, this bit will be cleared when writing
         * the next char to TDR register in the following call. */
        static_cast<Stm32f750Uart&>(System::getInstance().getUart(1))
            .onTransmitDataRegisterEmpty();
    }
}
}
