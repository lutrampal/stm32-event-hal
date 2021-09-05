
/*******************************************************************************
 * Interface file for timers of STM32F750
 ******************************************************************************/

#ifndef _HAL_DEVICE_STM32F750_TIMER_DEVICE_HPP
#define _HAL_DEVICE_STM32F750_TIMER_DEVICE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <chrono>
#include <cstdint>
#include <device/error_status.hpp>
#include <device/timer_device.hpp>
#include <functional>
#include <hardware/mcu.hpp>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class Stm32f750Timer : public TimerDevice
{
  public:
    Stm32f750Timer(TIM_TypeDef* hw_timer,
                   IRQn_Type irq_nb,
                   volatile uint32_t* clk_en_reg,
                   long clk_en_msk,
                   volatile uint32_t* rst_reg,
                   long rst_msk,
                   size_t counter_sz);
    ~Stm32f750Timer();

    WaitTimeUnitDuration getRemainingWaitTime() override;
    bool startWait(WaitTimeUnitDuration::rep count) override;
    bool suspendWait() override;
    bool cancelWait() override;
    bool resumeWait() override;
    bool completeWait() override;
    void usleep(WaitTimeUnitDuration::rep count) override;

  private:
    /* We want one tick per µs (=> CK_CNT = 1000000 Hz) */
    const unsigned ck_cnt = 1000000;

    TIM_TypeDef* const hw_timer;
    const IRQn_Type irq_nb;

    volatile uint32_t* const clk_en_reg;
    const long clk_en_msk;

    volatile uint32_t* const rst_reg;
    const long rst_msk;

    const WaitTimeUnitDuration::rep max_count;
};

}  // namespace device
}  // namespace hal

#endif
