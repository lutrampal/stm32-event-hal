
/*******************************************************************************
 * This code is responsible for:
 * - Copying mutable data from flash to SRAM
 * - Initializing vector table and copying handler code to ITCM
 * - Initializing SDRAM
 * - Initializing QSPI
 * - Handing over to main()
 ******************************************************************************/

#include <cstddef>
#include <device/stm32f750/stm32f750_irqs.hpp>
#include <hardware/mcu.hpp>

using namespace hal::device;

#define BOOT_CODE_ATTR __attribute__((section(".boot_code")))


extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _sidtcm, _sdtcm, _edtcm,
    _siitcm, _sitcm, _eitcm, _estack;

/* Array of init functions (constructors) to call before branching to main to
 * initialize static data. */
extern void (*_spreinit_array[])(void) __attribute__((weak));
extern void (*_epreinit_array[])(void) __attribute__((weak));
extern void (*_sinit_array[])(void) __attribute__((weak));
extern void (*_einit_array[])(void) __attribute__((weak));

extern "C" {
void __attribute__((naked)) __attribute__((section(".reset")))
handleReset(void);
}


/*******************************************************************************
 * STATIC CONSTANT DEFINITIONS
 ******************************************************************************/

static constexpr unsigned m_gpio_very_high_speed = 0x3;
static constexpr unsigned m_gpio_alternate_mode  = 0x2;
static uint8_t m_qspi_line_mode                  = 0x1;
static TIM_TypeDef* m_boot_timer                 = TIM2;


/*******************************************************************************
 * STATIC FUNCTION DECLARATIONS
 ******************************************************************************/

static void BOOT_CODE_ATTR m_initData(void);
static void BOOT_CODE_ATTR m_initVtable(void);
static void BOOT_CODE_ATTR m_setCoreSpeed(void);
static void BOOT_CODE_ATTR m_initQSPI();
static void BOOT_CODE_ATTR m_configureGPIO(GPIO_TypeDef* bank,
                                           unsigned pin,
                                           unsigned mode,
                                           unsigned ospeed,
                                           unsigned af);
static void BOOT_CODE_ATTR m_qspiReset();
static void BOOT_CODE_ATTR m_qspiEnableQuadIO();
static void BOOT_CODE_ATTR m_qspiEnableXIP();
static void BOOT_CODE_ATTR m_qspiMakeMemoryMapped();
static void BOOT_CODE_ATTR m_qspiWriteEnable();
static void BOOT_CODE_ATTR m_qspiWaitForRegister(uint8_t reg,
                                                 uint32_t msk,
                                                 uint32_t match);
static void BOOT_CODE_ATTR m_initFMC();
static void BOOT_CODE_ATTR m_initTimer(int32_t tick_time_us);
static void BOOT_CODE_ATTR m_usleep(uint32_t usec);
static void BOOT_CODE_ATTR m_cleanupTimer();

static constexpr uint32_t BOOT_CODE_ATTR m_nsToCycles(uint32_t ns,
                                                      uint32_t clk_hz);


/*******************************************************************************
 * STATIC FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

static void m_configureGPIO(GPIO_TypeDef* bank,
                            unsigned pin,
                            unsigned mode,
                            unsigned ospeed,
                            unsigned af)
{
    bank->MODER &= ~(0b11 << (pin * 2));
    bank->MODER |= (mode << (pin * 2));
    bank->OSPEEDR &= ~(0b11 << (pin * 2));
    bank->OSPEEDR |= (ospeed << (pin * 2));

    if (pin <= 7) {
        bank->AFR[0] &= ~(0b1111 << (pin * 4));
        bank->AFR[0] |= (af << (pin * 4));
    } else {
        bank->AFR[1] &= ~(0b1111 << ((pin - 8) * 4));
        bank->AFR[1] |= (af << ((pin - 8) * 4));
    }
}

static void m_usleep(uint32_t usec)
{
    uint32_t cnt_begin = m_boot_timer->CNT;
    uint32_t cnt_end   = cnt_begin + usec;
    while (m_boot_timer->CNT != cnt_end) {}
}

static void m_initTimer(int32_t tick_time_us)
{
    unsigned timer_idx =
        (reinterpret_cast<uintptr_t>(m_boot_timer) - APB1PERIPH_BASE)
        / 0x0400UL;
    unsigned timer_mask = 0b1 << timer_idx;
    RCC->APB1ENR |= timer_mask;
    /* Suspend the m_boot_timer when the core is halted by debug */
    DBGMCU->APB1FZ |= timer_mask;

    /* Disable m_boot_timer while we are configuring it */
    m_boot_timer->CR1 &= ~TIM_CR1_CEN;
    /* Reset m_boot_timer */
    RCC->APB1RSTR |= timer_mask;
    RCC->APB1RSTR &= ~timer_mask;

    uint32_t ck_cnt = 1000000 / tick_time_us;
    /* Increment the time counter every PSC + 1 clock ticks */
    m_boot_timer->PSC = (uint16_t)(core_clk_hz / ck_cnt - 1);
    /* Set ARR to max, we don't care about it since we won't generate IRQs */
    TIM2->ARR = UINT32_MAX;
    /* Reset the counter and apply new config */
    m_boot_timer->EGR |= TIM_EGR_UG;

    /* Enable TIM2 */
    m_boot_timer->CR1 |= TIM_CR1_CEN;
}

static void m_cleanupTimer()
{
    unsigned timer_idx =
        (reinterpret_cast<uintptr_t>(m_boot_timer) - APB1PERIPH_BASE)
        / 0x0400UL;
    unsigned timer_mask = 0b1 << timer_idx;
    m_boot_timer->CR1 &= ~TIM_CR1_CEN;
    DBGMCU->APB1FZ &= ~timer_mask;
    RCC->APB1ENR &= ~timer_mask;
}

static constexpr uint32_t m_nsToCycles(uint32_t ns, uint32_t clk_hz)
{
    uint32_t clk_mhz = clk_hz / 1000000;
    return (clk_mhz * ns) / 1000;
}

static void m_initFMC()
{
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
    /* Enable GPIO banks B, C, D, E, F, G & H used to map FMC pins
     * Refer to MCU datasheet */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN
                    | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN
                    | RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOGEN
                    | RCC_AHB1ENR_GPIOHEN;

    /* Configure GPIOs and switch them to FMC alternate function */
    m_configureGPIO(FMC_A0_GPIO_Port, FMC_A0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A1_GPIO_Port, FMC_A1_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A2_GPIO_Port, FMC_A2_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A3_GPIO_Port, FMC_A3_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A4_GPIO_Port, FMC_A4_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A5_GPIO_Port, FMC_A5_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A6_GPIO_Port, FMC_A6_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A7_GPIO_Port, FMC_A7_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A8_GPIO_Port, FMC_A8_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A9_GPIO_Port, FMC_A9_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A10_GPIO_Port, FMC_A10_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_A11_GPIO_Port, FMC_A11_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    // A12 not wired

    m_configureGPIO(FMC_D0_GPIO_Port, FMC_D0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D1_GPIO_Port, FMC_D1_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D2_GPIO_Port, FMC_D2_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D3_GPIO_Port, FMC_D3_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D4_GPIO_Port, FMC_D4_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D5_GPIO_Port, FMC_D5_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D6_GPIO_Port, FMC_D6_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D7_GPIO_Port, FMC_D7_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D8_GPIO_Port, FMC_D8_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D9_GPIO_Port, FMC_D9_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D10_GPIO_Port, FMC_D10_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D11_GPIO_Port, FMC_D11_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D12_GPIO_Port, FMC_D12_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D13_GPIO_Port, FMC_D13_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D14_GPIO_Port, FMC_D14_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_D15_GPIO_Port, FMC_D15_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    // D16 to D31 not wired

    m_configureGPIO(FMC_NBL0_GPIO_Port, FMC_NBL0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_NBL1_GPIO_Port, FMC_NBL1_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    // NBL2 & NBL3 not wired
    m_configureGPIO(FMC_SDCLK_GPIO_Port, FMC_SDCLK_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_SDNWE_GPIO_Port, FMC_SDNWE_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_SDNRAS_GPIO_Port, FMC_SDNRAS_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_SDNCAS_GPIO_Port, FMC_SDNCAS_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    m_configureGPIO(FMC_SDCKE0_GPIO_Port, FMC_SDCKE0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    // SDCKE1 not wired
    m_configureGPIO(FMC_SDNE0_GPIO_Port, FMC_SDNE0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, fmc_gpio_af12);
    // SDNE1 not wired

    /* SDRAM init: step 1, set FMC_SDCRx register(s) */
    /* No delay after CAS to read */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_RPIPE;
    FMC_Bank5_6->SDCR[0] |= 0b00 << FMC_SDCR1_RPIPE_Pos;
    /* Disable burst read */
    FMC_Bank5_6->SDCR[0] |= FMC_SDCR1_RBURST;
    /* 2 CPU clock cycles per SDRAM clock cycle */
    uint32_t sdclk_div = 2;
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_SDCLK;
    FMC_Bank5_6->SDCR[0] |= sdclk_div << FMC_SDCR1_SDCLK_Pos;
    /* Allow write access */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_WP;
    /* 3 CPU clock cycles for CAS latency */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_CAS;
    FMC_Bank5_6->SDCR[0] |= 0b11 << FMC_SDCR1_CAS_Pos;
    /* 4 internal banks for our module */
    FMC_Bank5_6->SDCR[0] |= FMC_SDCR1_NB;
    /* 16-bit data bus */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_MWID;
    FMC_Bank5_6->SDCR[0] |= 0b01 << FMC_SDCR1_MWID_Pos;
    /* 12-bit row addresses */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_NR;
    FMC_Bank5_6->SDCR[0] |= 0b01 << FMC_SDCR1_NR_Pos;
    /* 8-bit column addresses */
    FMC_Bank5_6->SDCR[0] &= ~FMC_SDCR1_NC;
    FMC_Bank5_6->SDCR[0] |= 0b00 << FMC_SDCR1_NC_Pos;

    /* SDRAM init: step 2, set FMC_SDTRx register(s) (module timings) */
    /* Timing for CAS latency = 3, cf p17 of SDRAM datasheet,
     * /!\ 0 => 1 cycle */
    uint32_t sdclk_hz = core_clk_hz / sdclk_div;
    FMC_Bank5_6->SDTR[0] &= ~FMC_SDTR1_TRCD & ~FMC_SDTR1_TRP & ~FMC_SDTR1_TWR
                            & ~FMC_SDTR1_TRC & ~FMC_SDTR1_TRAS & ~FMC_SDTR1_TXSR
                            & ~FMC_SDTR1_TMRD;
    FMC_Bank5_6->SDTR[0] |=
        ((m_nsToCycles(sdram_trcd_ns, sdclk_hz) - 1) << FMC_SDTR1_TRCD_Pos)
        | ((m_nsToCycles(sdram_trp_ns, sdclk_hz) - 1) << FMC_SDTR1_TRP_Pos)
        | ((m_nsToCycles(sdram_twr_ns, sdclk_hz) - 1) << FMC_SDTR1_TWR_Pos)
        | ((m_nsToCycles(sdram_trc_ns, sdclk_hz) - 1) << FMC_SDTR1_TRC_Pos)
        | ((m_nsToCycles(sdram_tras_ns, sdclk_hz) - 1) << FMC_SDTR1_TRAS_Pos)
        | ((m_nsToCycles(sdram_txsr_ns, sdclk_hz) - 1) << FMC_SDTR1_TXSR_Pos)
        | ((m_nsToCycles(sdram_tmrd_ns, sdclk_hz) - 1) << FMC_SDTR1_TMRD_Pos);

    /* SDRAM init: step 3, issue clock config enable command */
    FMC_Bank5_6->SDCMR &= ~FMC_SDCMR_MODE;
    FMC_Bank5_6->SDCMR |= FMC_SDCMR_CTB1 | (0b001 << FMC_SDCMR_MODE_Pos);

    /* SDRAM init: step 4, wait for command execution */
    m_usleep(100);
    /* Make sure command is executed */
    while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) {}

    /* SDRAM init: step 5, issue precharge all command */
    FMC_Bank5_6->SDCMR &= ~FMC_SDCMR_MODE;
    FMC_Bank5_6->SDCMR |= FMC_SDCMR_CTB1 | (0b010 << FMC_SDCMR_MODE_Pos);
    while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) {}

    /* SDRAM init: step 6, issue auto-refresh commands */
    /* Send 2 AF cmd (cf init procedure in SDRAM datasheet) */
    FMC_Bank5_6->SDCMR &= ~FMC_SDCMR_MODE & ~FMC_SDCMR_NRFS;
    FMC_Bank5_6->SDCMR |= FMC_SDCMR_CTB1 | (1 << FMC_SDCMR_NRFS_Pos)
                          | (0b011 << FMC_SDCMR_MODE_Pos);
    while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) {}

    /* SDRAM init: step 7, issue load mode register command */
    /* burst length = 1 = 0b000, burst type = 0, CAS latency = 3, operating mode
     * = 0, write burst mode = 0 */
    FMC_Bank5_6->SDCMR &= ~FMC_SDCMR_MODE & ~FMC_SDCMR_MRD;
    FMC_Bank5_6->SDCMR |= FMC_SDCMR_CTB1
                          | ((3 << MRD_LatencyMode_Pos) << FMC_SDCMR_MRD_Pos)
                          | (0b100 << FMC_SDCMR_MODE_Pos);
    while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) {}

    /* SDRAM init: step 8, set refresh rate */
    uint32_t ram_refresh_period_ns = 64000000;  // cf SDRAM datasheet
    uint32_t nb_rows               = 4096;
    uint32_t margin                = 20;  // as per ref manual
    uint32_t count =
        m_nsToCycles(ram_refresh_period_ns / nb_rows, sdclk_hz) - margin;
    FMC_Bank5_6->SDRTR &= ~FMC_SDRTR_COUNT;
    FMC_Bank5_6->SDRTR |= count << FMC_SDRTR_COUNT_Pos;
}

static void m_qspiWaitForRegister(uint8_t reg, uint32_t msk, uint32_t match)
{
    QUADSPI->CR |= QUADSPI_CR_EN;
    /* Set the 'mask', 'match', and 'polling interval' values. */
    QUADSPI->PSMKR = msk;
    QUADSPI->PSMAR = match;
    /* Poll every 16 clk */
    QUADSPI->PIR = 0x10;
    /* Set the 'auto-stop' bit to end the transaction after a match. */
    QUADSPI->CR |= QUADSPI_CR_APMS;
    /* Set instruction and data modes, and switch to auto-polling mode. */
    QUADSPI->CCR =
        ((m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
         | (m_qspi_line_mode << QUADSPI_CCR_DMODE_Pos)
         | (2 << QUADSPI_CCR_FMODE_Pos) | (reg << QUADSPI_CCR_INSTRUCTION_Pos));
    /* Wait for a match. */
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {};
    /* Clear status match flag */
    QUADSPI->FCR |= QUADSPI_FCR_CSMF;
    /* Disable the peripheral. */
    QUADSPI->CR &= ~QUADSPI_CR_EN;
}

static void m_qspiWriteEnable()
{
    QUADSPI->CR |= QUADSPI_CR_EN;
    /* comm config for write-enable: only the instruction phase is present,
     * instruction code = 0x6 */
    QUADSPI->CCR = (m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
                   | (0x6 << QUADSPI_CCR_INSTRUCTION_Pos);
    /* Wait for the transaction to finish. */
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {};
    /* Disable the peripheral. */
    QUADSPI->CR &= ~QUADSPI_CR_EN;
    /* Wait until write enabled is set in the status register */
    m_qspiWaitForRegister(0x05, 0x2, 0x2);
}


static void m_qspiMakeMemoryMapped()
{
    /* Comm cfg:
     *  - Enable SIOO (=> requires to send one null byte in alternate phase)
     *  - memory mapped mode
     *  - 10 dummy cycles (no need to configure the chip, this is the
     default)
     *      NB: we set 8 dummy clk in CCR because we're sending 8 alternative
     *          bits (over 4 wires i.e. 2 clk) after the address phase
     *  - 24-bit addresses
     *  - fast read instruction = 0x0B */
    QUADSPI->ABR = 0;
    QUADSPI->CCR = QUADSPI_CCR_SIOO | (0x3 << QUADSPI_CCR_FMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_DMODE_Pos)
                   | (8 << QUADSPI_CCR_DCYC_Pos) | (0 << QUADSPI_CCR_ABSIZE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_ABMODE_Pos)
                   | (0x2 << QUADSPI_CCR_ADSIZE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_ADMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
                   | (0xEB << QUADSPI_CCR_INSTRUCTION_Pos);
    QUADSPI->CR |= (QUADSPI_CR_EN);
}

static void m_qspiEnableXIP()
{
    m_qspiWriteEnable();
    /* we need to enable XIP in the VOLATILE CONFIGURATION REGISTER */
    QUADSPI->CR |= QUADSPI_CR_EN;
    /* 1 byte to write */
    QUADSPI->DLR = 0;
    /* Comm cfg
     *  - indirect write
     *  - no addr, no dummy cycle, etc.. only data
     *  - instruction code = 0x81 */
    QUADSPI->CCR = (0 << QUADSPI_CCR_FMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_DMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
                   | (0x81 << QUADSPI_CCR_INSTRUCTION_Pos);
    /* New register value, 10 dummy clock cycles, switch to XIP, all other
    bits
     * are default values */
    QUADSPI->DR = 0xF3;
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {}
}

static void m_qspiEnableQuadIO()
{
    m_qspiWriteEnable();
    /*** INIT ***/
    /* First, we need to enable quad I/O. To do this, we have to write the
     * ENHANCED VOLATILE CONFIGURATION REGISTER */
    QUADSPI->CR |= QUADSPI_CR_EN;
    /* Now we can send the command: */
    /* 1 byte to write */
    QUADSPI->DLR = 0;
    /* Comm cfg
     *  - indirect write
     *  - no addr, no dummy cycle, etc.. only data
     *  - instruction code = 0x61 */
    QUADSPI->CCR = (0 << QUADSPI_CCR_FMODE_Pos) | (1 << QUADSPI_CCR_DMODE_Pos)
                   | (1 << QUADSPI_CCR_IMODE_Pos)
                   | (0x61 << QUADSPI_CCR_INSTRUCTION_Pos);
    /* New register value, switch to quad line mode, all other bits are
    default
     * values */
    QUADSPI->DR = 0x7F;
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {}
    QUADSPI->CR &= ~QUADSPI_CR_EN;

    m_qspi_line_mode = 0x3;
}

static void m_qspiReset()
{
    /* RESET ENABLE command */
    QUADSPI->CR |= QUADSPI_CR_EN;
    /* Comm cfg
     *  - indirect write
     *  - no addr, no dummy cycle, no data
     *  - instruction code = 0x66 */
    QUADSPI->CCR = (0 << QUADSPI_CCR_FMODE_Pos) | (0 << QUADSPI_CCR_DMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
                   | (0x66 << QUADSPI_CCR_INSTRUCTION_Pos);
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {}

    /* RESET MEMORY command */
    /* Comm cfg
     *  - indirect write
     *  - no addr, no dummy cycle, no data
     *  - instruction code = 0x99 */
    QUADSPI->CCR = (0 << QUADSPI_CCR_FMODE_Pos) | (0 << QUADSPI_CCR_DMODE_Pos)
                   | (m_qspi_line_mode << QUADSPI_CCR_IMODE_Pos)
                   | (0x99 << QUADSPI_CCR_INSTRUCTION_Pos);
    while (QUADSPI->SR & QUADSPI_SR_BUSY) {}
}

static void m_initData(void)
{
    size_t data_size =
        reinterpret_cast<size_t>(&_edata) - reinterpret_cast<size_t>(&_sdata);
    for (size_t i = 0; i < data_size; ++i) {
        reinterpret_cast<uint8_t*>(&_sdata)[i] =
            reinterpret_cast<uint8_t*>(&_sidata)[i];
    }

    size_t dtcm_size =
        reinterpret_cast<size_t>(&_edtcm) - reinterpret_cast<size_t>(&_sdtcm);
    for (size_t i = 0; i < dtcm_size; ++i) {
        reinterpret_cast<uint8_t*>(&_sdtcm)[i] =
            reinterpret_cast<uint8_t*>(&_sidtcm)[i];
    }

    size_t bss_size =
        reinterpret_cast<size_t>(&_ebss) - reinterpret_cast<size_t>(&_sbss);
    for (size_t i = 0; i < bss_size; ++i) {
        reinterpret_cast<uint8_t*>(&_sbss)[i] = 0;
    }
}

static void m_initVtable(void)
{
    SCB->VTOR = (uint32_t)&g_vtable;

    g_vtable[0] = reinterpret_cast<InterruptHandler>(_estack);
    g_vtable[1] = handleReset;
    /* All interrupts default to error handler */
    for (unsigned i = 2; i < nb_irqs; ++i) { g_vtable[i] = handleError; }

    g_vtable[TIM2_IRQn + vtable_offset] = handleTIM2Event;
    g_vtable[TIM5_IRQn + vtable_offset] = handleTIM5Event;
}

static void m_setCoreSpeed(void)
{
    /* Before increasing the CPU speed, we need to set the read access
    latency
     * to allow the CPU to read from flash memory without error */

    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    /* Enable ART-accelerator, enable prefetch and set 9 wait states */
    FLASH->ACR |= FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_9WS;
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_9WS) {}

    /* After increasing the latency, we may change the clock speed */

    /* We'll use the PLL (=Phase-Locked Loop) for clock source as it allows
    for
     * the highest clock speed. First, we need to configure it */

    /* The PLL uses as clock source an external oscillator (HSE) which
    frequency
     * it will multiply to get a faster clock to the CPU. We need to enable
     the
     * HSE first and wait for it to be ready. */
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY) {}

    /* Select HSE a clock source for PLL & set the various multipliers /
     * dividers */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM & ~RCC_PLLCFGR_PLLN & ~RCC_PLLCFGR_PLLP;
    RCC->PLLCFGR |= (pllm << RCC_PLLCFGR_PLLM_Pos)
                    | (plln << RCC_PLLCFGR_PLLN_Pos)
                    | (pllq << RCC_PLLCFGR_PLLQ_Pos) | RCC_PLLCFGR_PLLSRC_HSE;

    /* The PLL is now setup to run at full speed.
     * We may now enable it and fait for it to be ready */
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY) {}

    /* Finally, select the PLL as clock source for the CPU.
     * Do not divide the incoming clock */
    RCC->CFGR &= ~RCC_CFGR_SW & ~RCC_CFGR_HPRE;
    RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_HPRE_DIV1;
    while ((RCC->CFGR & RCC_CFGR_HPRE) != RCC_CFGR_HPRE_DIV1
           && (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}

static void m_initQSPI()
{
    /* Enable QSPI clock */
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;

    /* Enable port clocks */
    RCC->AHB1ENR |=
        RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;
    /* Map ports to flash */
    m_configureGPIO(QSPI_CLK_GPIO_Port, QSPI_CLK_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af9);
    m_configureGPIO(QSPI_D0_GPIO_Port, QSPI_D0_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af9);
    m_configureGPIO(QSPI_D1_GPIO_Port, QSPI_D1_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af9);
    m_configureGPIO(QSPI_D2_GPIO_Port, QSPI_D2_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af9);
    m_configureGPIO(QSPI_D3_GPIO_Port, QSPI_D3_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af9);
    m_configureGPIO(QSPI_NCS_GPIO_Port, QSPI_NCS_Pin, m_gpio_alternate_mode,
                    m_gpio_very_high_speed, qspi_gpio_af10);

    /*** CONFIG ***/
    /* Number of bits required to address the QSPI - 1, the QSPI is 16MB long
     * so we need 24 bits */
    QUADSPI->DCR &= ~QUADSPI_DCR_FSIZE;
    QUADSPI->DCR |= 23 << QUADSPI_DCR_FSIZE_Pos;
    /* Set the memory speed: the flash will use the core clk divided by this
     * prescaler + 1. Here, we'll divide the core clk by 2 to get the QSPI
     * running at 108 MHz */
    QUADSPI->CR &= ~QUADSPI_CR_PRESCALER;
    QUADSPI->CR |= (1 << QUADSPI_CR_PRESCALER_Pos) | QUADSPI_CR_SSHIFT;

    m_qspiReset();
    m_qspiEnableQuadIO();
    m_qspiEnableXIP();
    m_qspiMakeMemoryMapped();
}

static void m_initStaticObjects()
{
    size_t pre_init_sz = _epreinit_array - _spreinit_array;
    for (size_t i = 0; i < pre_init_sz; ++i) { _spreinit_array[i](); }

    size_t init_sz = _einit_array - _sinit_array;
    for (size_t i = 0; i < init_sz; ++i) { _sinit_array[i](); }
}


/*******************************************************************************
 * EXTERN FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

void handleReset(void)
{
    __asm__("LDR r0, =_estack\n\t"
            "MOV sp, r0\n\t");
    /* Do not break debugger when in standy or sleep mode */
#ifdef DEBUG
    DBGMCU->CR |= DBGMCU_CR_DBG_STANDBY | DBGMCU_CR_DBG_SLEEP;

    /* Suspend all timers when the core is halted by debug */
    DBGMCU->APB1FZ |=
        DBGMCU_APB1_FZ_DBG_TIM2_STOP | DBGMCU_APB1_FZ_DBG_TIM3_STOP
        | DBGMCU_APB1_FZ_DBG_TIM4_STOP | DBGMCU_APB1_FZ_DBG_TIM5_STOP
        | DBGMCU_APB1_FZ_DBG_TIM6_STOP | DBGMCU_APB1_FZ_DBG_TIM7_STOP
        | DBGMCU_APB1_FZ_DBG_TIM12_STOP | DBGMCU_APB1_FZ_DBG_TIM13_STOP
        | DBGMCU_APB1_FZ_DBG_TIM14_STOP;
    DBGMCU->APB2FZ |=
        DBGMCU_APB2_FZ_DBG_TIM1_STOP | DBGMCU_APB2_FZ_DBG_TIM8_STOP
        | DBGMCU_APB2_FZ_DBG_TIM9_STOP | DBGMCU_APB2_FZ_DBG_TIM10_STOP
        | DBGMCU_APB2_FZ_DBG_TIM11_STOP;
#endif
    m_initData();
    m_initVtable();
    m_initTimer(1);
    m_setCoreSpeed();
    m_initQSPI();
    m_initFMC();
    m_cleanupTimer();
    m_initStaticObjects();

    __asm__("B main");
}
