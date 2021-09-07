
/*******************************************************************************
 * Implementation file for timers of STM32F750
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_timer.hpp"

#include <device/exceptions/timer_exceptions.hpp>
#include <hardware/mcu.hpp>

using namespace std;
using namespace hal::device;


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

Stm32f750Timer::Stm32f750Timer(TIM_TypeDef* hw_timer,
                               IRQn_Type irq_nb,
                               volatile uint32_t* clk_en_reg,
                               uint32_t clk_en_msk,
                               volatile uint32_t* rst_reg,
                               uint32_t rst_msk,
                               size_t counter_sz)
: hw_timer{hw_timer}, irq_nb{irq_nb}, clk_en_reg{clk_en_reg},
  clk_en_msk{clk_en_msk}, rst_reg{rst_reg}, rst_msk{rst_msk},
  max_count{(2UL << counter_sz) - 1UL}
{
    NVIC_SetPriority(irq_nb, 0);
    *clk_en_reg |= clk_en_msk;

    /* Disable timer while we are configuring it */
    hw_timer->CR1 &= ~TIM_CR1_CEN;
    /* Reset timer */
    *rst_reg |= rst_msk;
    *rst_reg &= ~rst_msk;

    /* Increment the time counter every PSC + 1 clock ticks */
    hw_timer->PSC = (uint16_t)(core_clk_hz / ck_cnt - 1);
    /* Enable IRQ generation based on TIM2 events */
    hw_timer->DIER |= TIM_DIER_UIE;
}

Stm32f750Timer::~Stm32f750Timer()
{
    NVIC_DisableIRQ(irq_nb);
    hw_timer->DIER &= ~TIM_DIER_UIE;
    hw_timer->CR1 &= ~TIM_CR1_CEN;
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

TimerDevice::WaitTimeUnitDuration Stm32f750Timer::getRemainingWaitTime()
{
    return TimerDevice::WaitTimeUnitDuration{programmed_count - hw_timer->CNT};
}

void Stm32f750Timer::usleep(WaitTimeUnitDuration::rep count)
{
    if (count > max_count) {
        throw InvalidTimerCountException{count, max_count};
    }

    /* Set the event period:
     * A TIM2 event is generated when the counter is equal to ARR */
    hw_timer->ARR    = count;
    programmed_count = count;
    /* URS is set to avoid generating interrupt when we set UG, OPM is used to
     * turn off the counter once it went off. */
    hw_timer->CR1 |= TIM_CR1_OPM | TIM_CR1_URS;
    /* Reset the counter and apply new config */
    hw_timer->EGR |= TIM_EGR_UG;
    /* Enable TIM2 */
    hw_timer->CR1 |= TIM_CR1_CEN;

    while ((hw_timer->SR & TIM_SR_UIF) == 0) {}

    hw_timer->SR &= ~TIM_SR_UIF;
}

bool Stm32f750Timer::startWait(WaitTimeUnitDuration::rep count)
{
    if (count > max_count) {
        throw InvalidTimerCountException{count, max_count};
    }

    NVIC_EnableIRQ(irq_nb);
    /* Set the event period:
     * A timer event is generated when the counter is equal to ARR */
    hw_timer->ARR    = count;
    programmed_count = count;
    /* URS is set to avoid generating interrupt when we set UG, OPM is used to
     * turn off the counter once it went off. */
    hw_timer->CR1 |= TIM_CR1_OPM | TIM_CR1_URS;
    /* Reset the counter and apply new config */
    hw_timer->EGR |= TIM_EGR_UG;
    /* Enable timer */
    hw_timer->CR1 |= TIM_CR1_CEN;

    return true;
}

bool Stm32f750Timer::suspendWait()
{
    hw_timer->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(irq_nb);

    return true;
}

bool Stm32f750Timer::cancelWait()
{
    /* Nothing to do in particular aside from suspending, settings will be
     * erased by next startWait() call */
    hw_timer->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(irq_nb);

    return true;
}

bool Stm32f750Timer::resumeWait()
{
    NVIC_EnableIRQ(irq_nb);
    hw_timer->CR1 |= TIM_CR1_CEN;

    return true;
}

bool Stm32f750Timer::onUpdateInterrupt()
{
    /* No need to disable the timer as we've set TIM_CR1_OPM, IRQ is cleared by
     * IRQ handler */
    if (wait_complete_callback) {
        wait_complete_callback(ErrorStatus{ErrorCode::Success});
    }

    return true;
}
