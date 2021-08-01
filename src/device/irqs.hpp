
/*******************************************************************************
 * Control global level interrupts
 ******************************************************************************/

#ifndef _HAL_DEVICE_IRQS_HPP
#define _HAL_DEVICE_IRQS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "../hardware/mcu.hpp"


/*******************************************************************************
 * DEFINE DIRECTIVES
 ******************************************************************************/

namespace hal
{
namespace device
{
/*******************************************************************************
 * PUBLIC TYPE DEFINITIONS
 ******************************************************************************/

typedef void (*InterruptHandler)(void);


/*******************************************************************************
 * EXTERN CONSTANT DECLARATIONS
 ******************************************************************************/

extern "C" {
__attribute__((aligned(0x200))) __attribute__((section(
    ".dtcm_vtable"))) extern volatile InterruptHandler g_vtable[nb_irqs];


/*******************************************************************************
 * EXTERN FUNCTION DECLARATIONS
 ******************************************************************************/

void handleError(void);
void handleTIM2Event(void);
void handleRNGEvent(void);
}

inline void enableInterrupts()
{
    __enable_irq();
}

inline void disableInterrupts()
{
    __disable_irq();
}


}  // namespace device
}  // namespace hal

#endif
