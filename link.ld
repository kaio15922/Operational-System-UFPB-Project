ENTRY(loader)

SECTIONS {
    . = 0xC0100000;

    /* --- INÍCIO DO CAPÍTULO 10 --- */
    /* Marca exatamente o endereço onde o kernel começa */
    kernel_virtual_start = .;
    kernel_physical_start = . - 0xC0000000;
    /* --- FIM DO CAPÍTULO 10 --- */

    /* A seção de código usa ADDR(.text) */
    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }

    /* A seção de strings usa ADDR(.rodata) */
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata*)
    }

    /* A seção de variáveis usa ADDR(.data) */
    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    /* A seção de variáveis não inicializadas usa ADDR(.bss) */
    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }

    /* --- INÍCIO DO CAPÍTULO 10 --- */
    /* Marca exatamente o endereço onde o kernel acaba */
    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
    /* --- FIM DO CAPÍTULO 10 --- */
}
