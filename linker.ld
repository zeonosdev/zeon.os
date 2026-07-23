/* ============================================
 * ZEONOS v0.3.0 - LINKER.LD
 * Target Architecture: x86 (IA-32) Bare-Metal
 * Description: Linker script for kernel memory layout
 * ============================================ */

ENTRY(_start)

SECTIONS
{
    /* Load kernel starting at 1MB (standard multiboot location) */
    . = 1M;

    /* Multiboot header and code segment */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data segment */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data segment (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data segment (uninitialized) & stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
