
/*******************************************************************************
 * Implementation file of IRQ handlers for STM32F750 MCU
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/irqs.hpp>
#include <hardware/mcu.hpp>

using namespace hal;
using namespace hal::device;

#ifdef MCU_STM32F750
extern "C" {
/*******************************************************************************
 * EXTERN VARIABLE DEFINITIONS
 ******************************************************************************/

__attribute__((aligned(0x200))) __attribute__((
    section(".dtcm_vtable"))) volatile InterruptHandler g_vtable[nb_irqs];


/*******************************************************************************
 * EXTERN FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

void handleError(void)
{
    while (1) {};
}
}

#endif
