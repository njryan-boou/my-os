ENTRY(kernel_entry)

OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x2000;

    .text :
    {
        KEEP(*(.text.entry))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        __bss_start = .;

        *(.bss*)
        *(COMMON)

        __bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}