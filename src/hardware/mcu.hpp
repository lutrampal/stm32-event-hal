
/*******************************************************************************
 * This file should be included in all modules that need direct access to the
 * hardware.
 ******************************************************************************/

#ifndef _HAL_HARDWARE_MCU_H
#define _HAL_HARDWARE_MCU_H

#ifdef MCU_STM32F750
    #include "stm32f750.hpp"
#else
    #error "Undefined MCU"
#endif

#endif
