
/*******************************************************************************
 * Implementation file of System class for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_timer.hpp"

#include <device/exceptions/system_exceptions.hpp>
#include <device/system.hpp>
#include <hardware/mcu.hpp>

using namespace std;
using namespace hal::device;


/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * STATIC FUNCTION DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 * STATIC FUNCTION DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

System::System()
{
}

System::~System()
{
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

System& System::getInstance()
{
    static System s;

    return s;
}

TimerDevice& System::getTimer(unsigned id)
{
    if (id < 1 || id > nb_timers) {
        throw InvalidTimerIdException(id);
    }

    /* IDs start at 1
     * Timer objects are not constructed at startup but only when they are
     * requested. */
    if (timers[id - 1] == nullptr) {
        switch (id) {
            case 1:
                timers[0] = make_unique<Stm32f750Timer>(
                    TIM1, TIM1_CC_IRQn, &RCC->APB2ENR, RCC_APB2ENR_TIM1EN,
                    &RCC->APB2RSTR, RCC_APB2RSTR_TIM1RST, 16);
                break;
            case 2:
                timers[1] = make_unique<Stm32f750Timer>(
                    TIM2, TIM2_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM2EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM2RST, 32);
                break;
            case 3:
                timers[2] = make_unique<Stm32f750Timer>(
                    TIM3, TIM3_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM3EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM3RST, 16);
                break;
            case 4:
                timers[3] = make_unique<Stm32f750Timer>(
                    TIM4, TIM4_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM4EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM4RST, 16);
                break;
            case 5:
                timers[4] = make_unique<Stm32f750Timer>(
                    TIM5, TIM5_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM5EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM5RST, 32);
                break;
            case 6:
                timers[5] = make_unique<Stm32f750Timer>(
                    TIM6, TIM6_DAC_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM6EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM6RST, 16);
                break;
            case 7:
                timers[6] = make_unique<Stm32f750Timer>(
                    TIM7, TIM7_IRQn, &RCC->APB1ENR, RCC_APB1ENR_TIM7EN,
                    &RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST, 16);
                break;
            case 8:
                timers[7] = make_unique<Stm32f750Timer>(
                    TIM8, TIM8_CC_IRQn, &RCC->APB2ENR, RCC_APB2ENR_TIM8EN,
                    &RCC->APB2RSTR, RCC_APB2RSTR_TIM8RST, 16);
                break;
            case 9:
                timers[8] = make_unique<Stm32f750Timer>(
                    TIM9, TIM1_BRK_TIM9_IRQn, &RCC->APB2ENR, RCC_APB2ENR_TIM9EN,
                    &RCC->APB2RSTR, RCC_APB2RSTR_TIM9RST, 16);
                break;
            case 10:
                timers[9] = make_unique<Stm32f750Timer>(
                    TIM10, TIM1_UP_TIM10_IRQn, &RCC->APB2ENR,
                    RCC_APB2ENR_TIM10EN, &RCC->APB2RSTR, RCC_APB2RSTR_TIM10RST,
                    16);
                break;
            case 11:
                timers[10] = make_unique<Stm32f750Timer>(
                    TIM11, TIM1_TRG_COM_TIM11_IRQn, &RCC->APB2ENR,
                    RCC_APB2ENR_TIM11EN, &RCC->APB2RSTR, RCC_APB2RSTR_TIM11RST,
                    16);
                break;
            case 12:
                timers[11] = make_unique<Stm32f750Timer>(
                    TIM12, TIM8_BRK_TIM12_IRQn, &RCC->APB1ENR,
                    RCC_APB1ENR_TIM12EN, &RCC->APB1RSTR, RCC_APB1RSTR_TIM12RST,
                    16);
                break;
            case 13:
                timers[12] = make_unique<Stm32f750Timer>(
                    TIM13, TIM8_UP_TIM13_IRQn, &RCC->APB1ENR,
                    RCC_APB1ENR_TIM13EN, &RCC->APB1RSTR, RCC_APB1RSTR_TIM13RST,
                    16);
                break;
            case 14:
                timers[13] = make_unique<Stm32f750Timer>(
                    TIM14, TIM8_TRG_COM_TIM14_IRQn, &RCC->APB1ENR,
                    RCC_APB1ENR_TIM14EN, &RCC->APB1RSTR, RCC_APB1RSTR_TIM14RST,
                    16);
                break;

            default:
                throw InvalidTimerIdException(id);
        }
    }

    return *timers[id - 1];
}
