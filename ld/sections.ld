
SECTIONS 
{
    .boot_code :
    {
        . = ALIGN(4);
        KEEP (*(.reset))
        KEEP (*(.boot_code))
        KEEP (*(.boot_code*))
        . = ALIGN(4);
    } >FLASH

    /* We can't store these in QSPI because then the jump would be too long */
    .ARM.exidx.flash :
    {
        . = ALIGN(4);
        *(.ARM.exidx.boot_code)
        *(.ARM.exidx.reset)
        *(.ARM.exidx.itcm_irqs)
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        KEEP (*(.init))
        KEEP (*(.fini))
        KEEP (*(.eh_frame_hdr))
        KEEP (*(.eh_frame))
        KEEP (*(.gcc_except_table))
        . = ALIGN(4);
    } >QSPI

    /* Sections required by the standard libraries. */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >QSPI

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >QSPI

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    /* Space for static writable, initialized to non 0 data */
    _sidata = .;
    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _sdata =  .;
        *(.data)
        *(.data*)
        _edata = .;
        . = ALIGN(4);
    } >SRAM

    /* Static, writable, initialized to 0 data */
    .bss (NOLOAD):
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >SRAM

    /* DTCM RAM region.
     * Tightly-coupled memory on the data bus.
     * We will store the interrupt vector table in this as it will constantly be
     * accessed */
    _sidtcm = _sidata + _edata - _sdata;
    .dtcm : AT(_sidtcm)
    {
        . = ALIGN(4);
        _sdtcm = .;
        KEEP (*(.dtcm_vtable))
        . = ALIGN(4);
        _edtcm = .;
    } >DTCMRAM_V

    .heap (NOLOAD): 
    {
        . = ALIGN(4);
        _sheap = .;
        end = _sheap;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        . = ALIGN(4);
        _eheap = .;
    } >SDRAM
} 
