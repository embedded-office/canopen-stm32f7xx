/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32F769NIHx Device with
**                2048KByte FLASH, 512KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Ac6.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Ac6 permit registered System Workbench for MCU users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the System Workbench for MCU toolchain.
**
*****************************************************************************
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20080000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
    core_intvec(rx) : ORIGIN = 0x08000000,   LENGTH = 0x00000200   /*   Intvec location           */
    core_rom   (rx) : ORIGIN = 0x08000200,   LENGTH = 0x001FFE00   /*   2M Demo Code              */

    core_ram_1 (rw) : ORIGIN = 0x20000000,   LENGTH = 0x00020000   /*    128k DTCM-RAM            */
    core_ram_2 (rw) : ORIGIN = 0x20020000,   LENGTH = 0x0005C000   /*    368k SRAM1               */
    core_ram_3 (rw) : ORIGIN = 0x2007C000,   LENGTH = 0x00004000   /*     16k SRAM2               */
    core_ram_4 (rwx): ORIGIN = 0x00000000,   LENGTH = 0x00004000   /*     16k ITCM-RAM            */
}

stack_size = 0x200;

SECTIONS {
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > core_intvec

    .canopen : {
        . = ALIGN(4);
        co_*.obj(.text)
        co_*.obj(.text.*)
        . = ALIGN(4);
        co_*.obj(.rodata)
        co_*.obj(.rodata*)
        _ecanopen = .;
    } > core_rom

    .text : {
        CREATE_OBJECT_SYMBOLS
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
        *(.plt)
        *(.gnu.warning)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))
        
        . = ALIGN(4);
        _etext = .;
    } > core_rom
    
    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
    } > core_rom

    .ARM.extab    : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >core_rom
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > core_rom

    .preinit_array  :   {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > core_rom

    .init_array :   { 
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > core_rom
    
    .fini_array :   {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > core_rom
    
    _sidata = LOADADDR(.data);
    _eidata = .;

    .data : AT (_eidata) ALIGN(0x400) {
        _sdata = .;
            /* privileged initialized data */
            *(.data)
            *(.data.*)
            *(.gnu.linkonce.d.*)
            
        . = ALIGN(4);
         _edata = .; 
        __data_bss = .;
    } > core_ram_1

    .bss __data_bss (NOLOAD) : {
            _sbss = .;
            __bss_start__ = _sbss;
            /* privileged bss */
            *(.shbss)
            *(.bss)
            *(.bss.*)
            *(.gnu.linkonce.b.*)
            *(COMMON)
            . = ALIGN (4);
            _ebss = .;
        	__bss_end__ = _ebss;
    } > core_ram_1

    .stack (NOLOAD) : {
        . = ALIGN(8);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN (8);
        __stack_end__ = .;
    } > core_ram_1

    PROVIDE(end = .);
    PROVIDE(_end = .);
}
