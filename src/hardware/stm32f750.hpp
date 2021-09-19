
/*******************************************************************************
 * Do not include direcly, use mcu.hpp instead
 ******************************************************************************/

#ifndef _STM32F750_H
#define _STM32F750_H


#ifdef BOARD_STM32F7508_DK
    #include "stm32f7508-dk.hpp"
#else
    #error "Undefined board"
#endif

#include <cstdint>


/** Offset to apply to each @ref IRQn_Type to get the actual handler index
 * in the vtable */
constexpr unsigned vtable_offset = 16;
/** Number of IRQS */
constexpr unsigned nb_irqs = 113;

/** CPU speed */
constexpr unsigned core_clk_hz = 216000000;
/** APB2 clock is setup in boot sequence and is derived from the core clock. */
constexpr unsigned apb2_clk_hz = core_clk_hz / 2;

/* The output clock of the PLL is given by:
 * VCO_out_clk = (VCO_in_clk / PLLM)*PLLN
 * PLL_clk = VCO_out_clk/PLLP */
constexpr unsigned pllm = hse_in_clk_hz / 1000000;
constexpr unsigned pllp = 2;
constexpr unsigned plln =
    (static_cast<uint64_t>(core_clk_hz) * static_cast<uint64_t>(pllp * pllm))
    / static_cast<uint64_t>(hse_in_clk_hz);
constexpr unsigned pllq = 9;

constexpr std::size_t nb_timers = 14;
constexpr std::size_t nb_uarts  = 8;
constexpr std::size_t nb_dmas   = 2;

constexpr uint32_t uart_baudrate = 115200;

#endif