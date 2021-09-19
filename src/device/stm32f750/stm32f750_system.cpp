
/*******************************************************************************
 * Implementation file of System class for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_dma.hpp"
#include "stm32f750_timer.hpp"
#include "stm32f750_uart.hpp"

#include <device/exceptions/system_exceptions.hpp>
#include <device/gpio_function.hpp>
#include <device/system.hpp>
#include <hardware/mcu.hpp>

using namespace std;
using namespace hal;
using namespace device;


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

CharacterDevice<char>& System::getUart(unsigned id)
{
    if (id < 1 || id > nb_uarts) {
        throw InvalidUartIdException(id);
    }

    /* IDs start at 1
     * UART objects are not constructed at startup but only when they are
     * requested.
     * We must also setup the GPIOs */
    if (uarts[id - 1] == nullptr) {
        switch (id) {
            case 1:
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* VCP_RX */
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /* VCP_TX */
                gpioFunctionConfigure(VCP_TX_GPIO_Port, VCP_TX_Pin,
                                      SelFunc::Alt7, PinSpeed::Medium);
                gpioFunctionConfigure(VCP_RX_GPIO_Port, VCP_RX_Pin,
                                      SelFunc::Alt7, PinSpeed::Medium);
                uarts[0] = make_unique<Stm32f750Uart>(
                    USART1, USART1_IRQn, &RCC->APB2ENR, RCC_APB2ENR_USART1EN,
                    uart_baudrate);
                break;
            case 2:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("USART2");
                uarts[1] = make_unique<Stm32f750Uart>(
                    USART2, USART2_IRQn, &RCC->APB1ENR, RCC_APB1ENR_USART2EN,
                    uart_baudrate);
                break;
            case 3:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("USART3");
                uarts[2] = make_unique<Stm32f750Uart>(
                    USART3, USART3_IRQn, &RCC->APB1ENR, RCC_APB1ENR_USART3EN,
                    uart_baudrate);
                break;
            case 4:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("UART4");
                uarts[3] = make_unique<Stm32f750Uart>(
                    UART4, UART4_IRQn, &RCC->APB1ENR, RCC_APB1ENR_UART4EN,
                    uart_baudrate);
                break;
            case 5:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("UART5");
                uarts[4] = make_unique<Stm32f750Uart>(
                    UART5, UART5_IRQn, &RCC->APB1ENR, RCC_APB1ENR_UART5EN,
                    uart_baudrate);
                break;
            case 6:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("USART6");
                uarts[5] = make_unique<Stm32f750Uart>(
                    USART6, USART6_IRQn, &RCC->APB2ENR, RCC_APB2ENR_USART6EN,
                    uart_baudrate);
                break;
            case 7:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("UART7");
                uarts[6] = make_unique<Stm32f750Uart>(
                    UART7, UART7_IRQn, &RCC->APB1ENR, RCC_APB1ENR_UART7EN,
                    uart_baudrate);
                break;
            case 8:
                /* TODO: GPIO setup */
                throw UnimplementedDeviceException("UART8");
                uarts[7] = make_unique<Stm32f750Uart>(
                    UART8, UART8_IRQn, &RCC->APB1ENR, RCC_APB1ENR_UART8EN,
                    uart_baudrate);
                break;

            default:
                throw InvalidUartIdException(id);
        }
    }

    return *uarts[id - 1];
}

DmaDevice& System::getDma(unsigned id)
{
    if (id < 1 || id > nb_dmas) {
        throw InvalidUartIdException(id);
    }

    /* IDs start at 1
     * DMA objects are not constructed at startup but only when they are
     * requested. */
    if (dmas[id - 1] == nullptr) {
        switch (id) {
            case 1:
                dmas[0] = make_unique<Stm32f750Dma>(
                    DMA1,
                    array<IRQn_Type, Stm32f750Dma::nb_streams>{
                        DMA1_Stream0_IRQn,
                        DMA1_Stream1_IRQn,
                        DMA1_Stream2_IRQn,
                        DMA1_Stream3_IRQn,
                        DMA1_Stream4_IRQn,
                        DMA1_Stream5_IRQn,
                        DMA1_Stream6_IRQn,
                        DMA1_Stream7_IRQn,
                    },
                    &RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN, &RCC->AHB1RSTR,
                    RCC_AHB1RSTR_DMA1RST);
                break;
            case 2:
                dmas[1] = make_unique<Stm32f750Dma>(
                    DMA2,
                    array<IRQn_Type, Stm32f750Dma::nb_streams>{
                        DMA2_Stream0_IRQn,
                        DMA2_Stream1_IRQn,
                        DMA2_Stream2_IRQn,
                        DMA2_Stream3_IRQn,
                        DMA2_Stream4_IRQn,
                        DMA2_Stream5_IRQn,
                        DMA2_Stream6_IRQn,
                        DMA2_Stream7_IRQn,
                    },
                    &RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN, &RCC->AHB1RSTR,
                    RCC_AHB1RSTR_DMA2RST);
                break;

            default:
                throw InvalidDmaIdException(id);
        }
    }

    return *dmas[id - 1];
}

EventLoop& System::getEventLoop()
{
    return event_loop;
}
