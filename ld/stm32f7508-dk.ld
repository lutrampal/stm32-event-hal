
ENTRY(handleReset)

/* Set the end of stack at the end of DTCMRAM (before interrupt vector table) */
_estack = 0x2000FC00;

/* Min size of stack & heap, 
 * The linker will generate error if there is less than this leftover. */
_min_ram_left = 0x400;

MEMORY
{
    FLASH       (rxw) : ORIGIN = 0x08000000, LENGTH = 64K
    ITCMRAM     (rxw) : ORIGIN = 0x00000000, LENGTH = 16K
    /* Split the DTCM RAM, we'll store the IRQ table right at the end of it and 
     * the rest will be dedicated to the stack. */
    DTCMRAM_S   (rxw) : ORIGIN = 0x20000000, LENGTH = 63K
    DTCMRAM_V   (rxw) : ORIGIN = 0x2000FC00, LENGTH = 1K
    SRAM        (rxw) : ORIGIN = 0x20010000, LENGTH = 256K
    QSPI        (rxw) : ORIGIN = 0x90000000, LENGTH = 16M
    SDRAM       (rxw) : ORIGIN = 0xC0000000, LENGTH = 8M
}

INCLUDE "sections.ld"
