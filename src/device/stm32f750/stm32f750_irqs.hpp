
/*******************************************************************************
 * Control global level interrupts. Each handler will request the matching
 * device from the System singleton and call the corresponding event method. IRQ
 * flags should be cleared by IRQs handler, **not** by event methods.
 ******************************************************************************/

#ifndef _HAL_DEVICE_STM32F750_IRQS_HPP
#define _HAL_DEVICE_STM32F750_IRQS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/irqs.hpp>
#include <hardware/mcu.hpp>


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


/*******************************************************************************
 * EXTERN CONSTANT DECLARATIONS
 ******************************************************************************/

extern "C" {


/*******************************************************************************
 * EXTERN FUNCTION DECLARATIONS
 ******************************************************************************/

void handleTIM2Event(void);
void handleTIM5Event(void);
void handleUSART1Event(void);
}
}  // namespace device
}  // namespace hal

#endif
