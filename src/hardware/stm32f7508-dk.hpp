
/*******************************************************************************
 * Do not include direcly, use mcu.hpp instead
 ******************************************************************************/

#ifndef STM32F7508_DK_H
#define STM32F7508_DK_H

extern "C" {
#include <stm32f750xx.h>
}


/* I/O assignment for the STM32F7508 development kit */
#define LCD_B0_Pin                     4
#define LCD_B0_GPIO_Port               GPIOE
#define OTG_HS_OverCurrent_Pin         3
#define OTG_HS_OverCurrent_GPIO_Port   GPIOE
#define QSPI_D2_Pin                    2
#define QSPI_D2_GPIO_Port              GPIOE
#define RMII_TXD1_Pin                  14
#define RMII_TXD1_GPIO_Port            GPIOG
#define FMC_NBL1_Pin                   1
#define FMC_NBL1_GPIO_Port             GPIOE
#define FMC_NBL0_Pin                   0
#define FMC_NBL0_GPIO_Port             GPIOE
#define ARDUINO_SCL_D15_Pin            8
#define ARDUINO_SCL_D15_GPIO_Port      GPIOB
#define ARDUINO_PWM_D3_Pin             4
#define ARDUINO_PWM_D3_GPIO_Port       GPIOB
#define SWO_Pin                        3
#define SWO_GPIO_Port                  GPIOB
#define SPDIF_RX0_Pin                  7
#define SPDIF_RX0_GPIO_Port            GPIOD
#define SDMMC_CK_Pin                   12
#define SDMMC_CK_GPIO_Port             GPIOC
#define USD_DETECT_Pin                 13
#define USD_DETECT_GPIO_Port           GPIOC
#define ARDUINO_PWM_D9_Pin             15
#define ARDUINO_PWM_D9_GPIO_Port       GPIOA
#define SWCLK_Pin                      14
#define SWCLK_GPIO_Port                GPIOA
#define SWDIO_Pin                      13
#define SWDIO_GPIO_Port                GPIOA
#define DCMI_D6_Pin                    5
#define DCMI_D6_GPIO_Port              GPIOE
#define DCMI_D7_Pin                    6
#define DCMI_D7_GPIO_Port              GPIOE
#define RMII_TXD0_Pin                  13
#define RMII_TXD0_GPIO_Port            GPIOG
#define ARDUINO_SDA_D14_Pin            9
#define ARDUINO_SDA_D14_GPIO_Port      GPIOB
#define VCP_RX_Pin                     7
#define VCP_RX_GPIO_Port               GPIOB
#define QSPI_NCS_Pin                   6
#define QSPI_NCS_GPIO_Port             GPIOB
#define FMC_SDNCAS_Pin                 15
#define FMC_SDNCAS_GPIO_Port           GPIOG
#define RMII_TX_EN_Pin                 11
#define RMII_TX_EN_GPIO_Port           GPIOG
#define LCD_B1_Pin                     13
#define LCD_B1_GPIO_Port               GPIOJ
#define OTG_FS_VBUS_Pin                12
#define OTG_FS_VBUS_GPIO_Port          GPIOJ
#define Audio_INT_Pin                  6
#define Audio_INT_GPIO_Port            GPIOD
#define FMC_D2_Pin                     0
#define FMC_D2_GPIO_Port               GPIOD
#define SDMMC_D3_Pin                   11
#define SDMMC_D3_GPIO_Port             GPIOC
#define SDMMC_D2_Pin                   10
#define SDMMC_D2_GPIO_Port             GPIOC
#define SDMMC_D1_Pin                   9
#define SDMMC_D1_GPIO_Port             GPIOC
#define SDMMC_D0_Pin                   8
#define SDMMC_D0_GPIO_Port             GPIOC
#define OTG_FS_P_Pin                   12
#define OTG_FS_P_GPIO_Port             GPIOA
#define NC1_Pin                        8
#define NC1_GPIO_Port                  GPIOI
#define SAI2_MCLKA_Pin                 4
#define SAI2_MCLKA_GPIO_Port           GPIOI
#define LCD_DE_Pin                     7
#define LCD_DE_GPIO_Port               GPIOK
#define LCD_B7_Pin                     6
#define LCD_B7_GPIO_Port               GPIOK
#define LCD_B6_Pin                     5
#define LCD_B6_GPIO_Port               GPIOK
#define LCD_B4_Pin                     12
#define LCD_B4_GPIO_Port               GPIOG
#define SAI2_SDB_Pin                   10
#define SAI2_SDB_GPIO_Port             GPIOG
#define LCD_B2_Pin                     14
#define LCD_B2_GPIO_Port               GPIOJ
#define OTG_FS_PowerSwitchOn_Pin       5
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOD
#define DCMI_D5_Pin                    3
#define DCMI_D5_GPIO_Port              GPIOD
#define FMC_D3_Pin                     1
#define FMC_D3_GPIO_Port               GPIOD
#define ARDUINO_D7_Pin                 3
#define ARDUINO_D7_GPIO_Port           GPIOI
#define ARDUINO_D8_Pin                 2
#define ARDUINO_D8_GPIO_Port           GPIOI
#define OTG_FS_N_Pin                   11
#define OTG_FS_N_GPIO_Port             GPIOA
#define uSD_Detect_Pin                 13
#define uSD_Detect_GPIO_Port           GPIOC
#define FMC_A0_Pin                     0
#define FMC_A0_GPIO_Port               GPIOF
#define SAI2_SCKA_Pin                  5
#define SAI2_SCKA_GPIO_Port            GPIOI
#define SAI2_FSA_Pin                   7
#define SAI2_FSA_GPIO_Port             GPIOI
#define LCD_HSYNC_Pin                  10
#define LCD_HSYNC_GPIO_Port            GPIOI
#define SAI2_SDA_Pin                   6
#define SAI2_SDA_GPIO_Port             GPIOI
#define LCD_B5_Pin                     4
#define LCD_B5_GPIO_Port               GPIOK
#define LCD_BL_CTRL_Pin                3
#define LCD_BL_CTRL_GPIO_Port          GPIOK
#define DCMI_VSYNC_Pin                 9
#define DCMI_VSYNC_GPIO_Port           GPIOG
#define LCD_B3_Pin                     15
#define LCD_B3_GPIO_Port               GPIOJ
#define OTG_FS_OverCurrent_Pin         4
#define OTG_FS_OverCurrent_GPIO_Port   GPIOD
#define SDMMC_CMD_Pin                  2
#define SDMMC_CMD_GPIO_Port            GPIOD
#define TP3_Pin                        15
#define TP3_GPIO_Port                  GPIOH
#define ARDUINO_SCK_D13_Pin            1
#define ARDUINO_SCK_D13_GPIO_Port      GPIOI
#define OTG_FS_ID_Pin                  10
#define OTG_FS_ID_GPIO_Port            GPIOA
#define RCC_OSC32_IN_Pin               14
#define RCC_OSC32_IN_GPIO_Port         GPIOC
#define FMC_A1_Pin                     1
#define FMC_A1_GPIO_Port               GPIOF
#define LCD_DISP_Pin                   12
#define LCD_DISP_GPIO_Port             GPIOI
#define LCD_VSYNC_Pin                  9
#define LCD_VSYNC_GPIO_Port            GPIOI
#define DCMI_PWR_EN_Pin                13
#define DCMI_PWR_EN_GPIO_Port          GPIOH
#define DCMI_D4_Pin                    14
#define DCMI_D4_GPIO_Port              GPIOH
#define ARDUINO_PWM_CS_D5_Pin          0
#define ARDUINO_PWM_CS_D5_GPIO_Port    GPIOI
#define VCP_TX_Pin                     9
#define VCP_TX_GPIO_Port               GPIOA
#define RCC_OSC32_OUT_Pin              15
#define RCC_OSC32_OUT_GPIO_Port        GPIOC
#define LCD_G6_Pin                     1
#define LCD_G6_GPIO_Port               GPIOK
#define LCD_G7_Pin                     2
#define LCD_G7_GPIO_Port               GPIOK
#define ARDUINO_PWM_D10_Pin            8
#define ARDUINO_PWM_D10_GPIO_Port      GPIOA
#define OSC_25M_Pin                    0
#define OSC_25M_GPIO_Port              GPIOH
#define FMC_A2_Pin                     2
#define FMC_A2_GPIO_Port               GPIOF
#define LCD_INT_Pin                    13
#define LCD_INT_GPIO_Port              GPIOI
#define LCD_R0_Pin                     15
#define LCD_R0_GPIO_Port               GPIOI
#define LCD_G4_Pin                     11
#define LCD_G4_GPIO_Port               GPIOJ
#define LCD_G5_Pin                     0
#define LCD_G5_GPIO_Port               GPIOK
#define ARDUINO_RX_D0_Pin              7
#define ARDUINO_RX_D0_GPIO_Port        GPIOC
#define FMC_A3_Pin                     3
#define FMC_A3_GPIO_Port               GPIOF
#define LCD_CLK_Pin                    14
#define LCD_CLK_GPIO_Port              GPIOI
#define LCD_G1_Pin                     8
#define LCD_G1_GPIO_Port               GPIOJ
#define LCD_G3_Pin                     10
#define LCD_G3_GPIO_Port               GPIOJ
#define FMC_SDCLK_Pin                  8
#define FMC_SDCLK_GPIO_Port            GPIOG
#define FMC_SDNWE_Pin                  5
#define FMC_SDNWE_GPIO_Port            GPIOH
#define ARDUINO_TX_D1_Pin              6
#define ARDUINO_TX_D1_GPIO_Port        GPIOC
#define FMC_A4_Pin                     4
#define FMC_A4_GPIO_Port               GPIOF
#define FMC_SDNME_Pin                  5
#define FMC_SDNME_GPIO_Port            GPIOH
#define FMC_SDNE0_Pin                  3
#define FMC_SDNE0_GPIO_Port            GPIOH
#define LCD_G0_Pin                     7
#define LCD_G0_GPIO_Port               GPIOJ
#define LCD_G2_Pin                     9
#define LCD_G2_GPIO_Port               GPIOJ
#define ARDUINO_D4_Pin                 7
#define ARDUINO_D4_GPIO_Port           GPIOG
#define ARDUINO_D2_Pin                 6
#define ARDUINO_D2_GPIO_Port           GPIOG
#define ARDUINO_A4_Pin                 7
#define ARDUINO_A4_GPIO_Port           GPIOF
#define ARDUINO_A5_Pin                 6
#define ARDUINO_A5_GPIO_Port           GPIOF
#define FMC_A5_Pin                     5
#define FMC_A5_GPIO_Port               GPIOF
#define NC2_Pin                        2
#define NC2_GPIO_Port                  GPIOH
#define LCD_R7_Pin                     6
#define LCD_R7_GPIO_Port               GPIOJ
#define FMC_D1_Pin                     15
#define FMC_D1_GPIO_Port               GPIOD
#define FMC_D15_Pin                    10
#define FMC_D15_GPIO_Port              GPIOD
#define ARDUINO_A1_Pin                 10
#define ARDUINO_A1_GPIO_Port           GPIOF
#define ARDUINO_A2_Pin                 9
#define ARDUINO_A2_GPIO_Port           GPIOF
#define ARDUINO_A3_Pin                 8
#define ARDUINO_A3_GPIO_Port           GPIOF
#define FMC_SDCKE0_Pin                 3
#define FMC_SDCKE0_GPIO_Port           GPIOC
#define FMC_D0_Pin                     14
#define FMC_D0_GPIO_Port               GPIOD
#define FMC_D14_Pin                    9
#define FMC_D14_GPIO_Port              GPIOD
#define FMC_D13_Pin                    8
#define FMC_D13_GPIO_Port              GPIOD
#define RMII_MDC_Pin                   1
#define RMII_MDC_GPIO_Port             GPIOC
#define FMC_A6_Pin                     12
#define FMC_A6_GPIO_Port               GPIOF
#define FMC_A12_Pin                    2
#define FMC_A12_GPIO_Port              GPIOG
#define FMC_A11_Pin                    1
#define FMC_A11_GPIO_Port              GPIOG
#define FMC_A9_Pin                     15
#define FMC_A9_GPIO_Port               GPIOF
#define LCD_R5_Pin                     4
#define LCD_R5_GPIO_Port               GPIOJ
#define QSPI_CLK_Pin                   2
#define QSPI_CLK_GPIO_Port             GPIOB
#define QSPI_D1_Pin                    12
#define QSPI_D1_GPIO_Port              GPIOD
#define QSPI_D3_Pin                    13
#define QSPI_D3_GPIO_Port              GPIOD
#define EXT_RST_Pin                    3
#define EXT_RST_GPIO_Port              GPIOG
#define RMII_RXER_Pin                  2
#define RMII_RXER_GPIO_Port            GPIOG
#define LCD_R6_Pin                     5
#define LCD_R6_GPIO_Port               GPIOJ
#define DCMI_D3_Pin                    12
#define DCMI_D3_GPIO_Port              GPIOH
#define RMII_REF_CLK_Pin               1
#define RMII_REF_CLK_GPIO_Port         GPIOA
#define ARDUINO_A0_Pin                 0
#define ARDUINO_A0_GPIO_Port           GPIOA
#define DCMI_HSYNC_Pin                 4
#define DCMI_HSYNC_GPIO_Port           GPIOA
#define RMII_RXD0_Pin                  4
#define RMII_RXD0_GPIO_Port            GPIOC
#define FMC_A7_Pin                     13
#define FMC_A7_GPIO_Port               GPIOF
#define FMC_A10_Pin                    0
#define FMC_A10_GPIO_Port              GPIOG
#define LCD_R4_Pin                     3
#define LCD_R4_GPIO_Port               GPIOJ
#define FMC_D5_Pin                     8
#define FMC_D5_GPIO_Port               GPIOE
#define QSPI_D0_Pin                    11
#define QSPI_D0_GPIO_Port              GPIOD
#define FMC_BA1_Pin                    5
#define FMC_BA1_GPIO_Port              GPIOG
#define FMC_BA0_Pin                    4
#define FMC_BA0_GPIO_Port              GPIOG
#define LCD_SCL_Pin                    7
#define LCD_SCL_GPIO_Port              GPIOH
#define DCMI_D0_Pin                    9
#define DCMI_D0_GPIO_Port              GPIOH
#define DCMI_D2_Pin                    11
#define DCMI_D2_GPIO_Port              GPIOH
#define RMII_MDIO_Pin                  2
#define RMII_MDIO_GPIO_Port            GPIOA
#define RMII_RXD1_Pin                  5
#define RMII_RXD1_GPIO_Port            GPIOC
#define FMC_A8_Pin                     14
#define FMC_A8_GPIO_Port               GPIOF
#define LCD_R3_Pin                     2
#define LCD_R3_GPIO_Port               GPIOJ
#define FMC_SDNRAS_Pin                 11
#define FMC_SDNRAS_GPIO_Port           GPIOF
#define FMC_D6_Pin                     9
#define FMC_D6_GPIO_Port               GPIOE
#define FMC_D8_Pin                     11
#define FMC_D8_GPIO_Port               GPIOE
#define FMC_D11_Pin                    14
#define FMC_D11_GPIO_Port              GPIOE
#define ULPI_D3_Pin                    10
#define ULPI_D3_GPIO_Port              GPIOB
#define ARDUINO_PWM_D6_Pin             6
#define ARDUINO_PWM_D6_GPIO_Port       GPIOH
#define LCD_SDA_Pin                    8
#define LCD_SDA_GPIO_Port              GPIOH
#define DCMI_D1_Pin                    10
#define DCMI_D1_GPIO_Port              GPIOH
#define RMII_CRS_DV_Pin                7
#define RMII_CRS_DV_GPIO_Port          GPIOA
#define LCD_R1_Pin                     0
#define LCD_R1_GPIO_Port               GPIOJ
#define LCD_R2_Pin                     1
#define LCD_R2_GPIO_Port               GPIOJ
#define FMC_D4_Pin                     7
#define FMC_D4_GPIO_Port               GPIOE
#define FMC_D7_Pin                     10
#define FMC_D7_GPIO_Port               GPIOE
#define FMC_D9_Pin                     12
#define FMC_D9_GPIO_Port               GPIOE
#define FMC_D12_Pin                    15
#define FMC_D12_GPIO_Port              GPIOE
#define FMC_D10_Pin                    13
#define FMC_D10_GPIO_Port              GPIOE
#define ARDUINO_MISO_D12_Pin           14
#define ARDUINO_MISO_D12_GPIO_Port     GPIOB
#define ARDUINO_MOSI_PWM_D11_Pin       15
#define ARDUINO_MOSI_PWM_D11_GPIO_Port GPIOB

/* Frequency of the oscillator used as input for the HSE */
constexpr unsigned hse_in_clk_hz = 25000000;

/* Alternate functions for GPIOs */
constexpr unsigned qspi_gpio_af9  = 9;
constexpr unsigned qspi_gpio_af10 = 10;
constexpr unsigned fmc_gpio_af12  = 12;
constexpr unsigned lcd_gpio_af14  = 14;

constexpr unsigned MRD_BurstLength_Pos    = 0;
constexpr unsigned MRD_BurstLength_Msk    = 0b111 << MRD_BurstLength_Pos;
constexpr unsigned MRD_BurstLength        = MRD_BurstLength_Msk;
constexpr unsigned MRD_BurstType_Pos      = 3;
constexpr unsigned MRD_BurstType_Msk      = 0b1 << MRD_BurstType_Pos;
constexpr unsigned MRD_BurstType          = MRD_BurstType_Msk;
constexpr unsigned MRD_LatencyMode_Pos    = 4;
constexpr unsigned MRD_LatencyMode_Msk    = 0b111 << MRD_LatencyMode_Pos;
constexpr unsigned MRD_LatencyMode        = MRD_LatencyMode_Msk;
constexpr unsigned MRD_OperatingMode_Pos  = 7;
constexpr unsigned MRD_OperatingMode_Msk  = 0b11 << MRD_OperatingMode_Pos;
constexpr unsigned MRD_OperatingMode      = MRD_OperatingMode_Msk;
constexpr unsigned MRD_WriteBurstMode_Pos = 9;
constexpr unsigned MRD_WriteBurstMode_Msk = 0b1 << MRD_WriteBurstMode_Pos;
constexpr unsigned MRD_WriteBurstMode     = MRD_WriteBurstMode_Msk;

constexpr unsigned sdram_trcd_ns = 18;
constexpr unsigned sdram_trp_ns  = 18;
constexpr unsigned sdram_twr_ns  = 24;
constexpr unsigned sdram_trc_ns  = 60;
constexpr unsigned sdram_tras_ns = 42;
constexpr unsigned sdram_txsr_ns = 70;
constexpr unsigned sdram_tmrd_ns = 12;

constexpr unsigned screen_hsync  = 41;
constexpr unsigned screen_hbp    = 13;
constexpr unsigned screen_hfp    = 32;
constexpr unsigned screen_vsync  = 10;
constexpr unsigned screen_vbp    = 2;
constexpr unsigned screen_vfp    = 2;
constexpr unsigned screen_width  = 480;
constexpr unsigned screen_height = 272;
constexpr unsigned screen_hs_pol = 0;
constexpr unsigned screen_vs_pol = 0;
constexpr unsigned screen_pc_pol = 0;
constexpr unsigned screen_de_pol = 0;


#endif