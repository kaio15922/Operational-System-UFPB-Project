OUTPUT_FORMAT("binary")    /* output flat binary, não um ELF */

SECTIONS
{
    . = 0;                 /* realoca para o endereço 0 */

    .text ALIGN(4):
    {
        start.o(.text)     /* inclui a seção .text do start.o primeiro */
        *(.text)           /* inclui todas as outras seções .text (como as do seu C) */
    }

    .data ALIGN(4):
    {
        *(.data)
    }

    .rodata ALIGN(4):
    {
        *(.rodata*)
    }
}