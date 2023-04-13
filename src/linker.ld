ENTRY(_Reset)

MEMORY
{
    RAM (xrw) : ORIGIN = 0x60000000, LENGTH = 32M
}

heap_size = 0x10000; /* 64K */

SECTIONS {
    /* . = ORIGIN(RAM) + 0x10000; */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        _start_text = .;
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        KEEP(*(.eh_frame*))
    } >RAM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >RAM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } >RAM
    __exidx_end = .;

    . = ALIGN(4);
    _end_text = .;

    .data :
    {
        _start_data = .;
        *(vtable)
        *(.data)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        _end_data = .;
    } >RAM

    . = ALIGN(4);
    .bss :
    {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } >RAM

    . = ALIGN(4);
    .heap :
    {
        __end__ = .;
        /* "end" is used by newlib's syscalls!!! */
        PROVIDE(end = .);
        PROVIDE(_heap_start = end );

        . = . + heap_size;

        PROVIDE(_heap_end = .);
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) } >RAM

    . = ALIGN(8);
    .stack : {
        __stack_start = .;

        . = . + 0x00001000;
        stackSVC = .;

        . = . + 0x00001000;
        stackUND = .;

        . = . + 0x00001000;
        stackABT = .;

        . = . + 0x00001000;
        stackIRQ = .;

        . = . + 0x00001000;
        stackFIQ = .;

        . = . + 0x00001000;
        stackUSR = .;

        __stack_end = .;
    } >RAM
}
_end = .;

