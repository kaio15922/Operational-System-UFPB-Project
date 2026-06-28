ENTRY(loader)		     /* Quando o SO inicia, exec a primeira inst. daqui */

SECTIONS {
    . = 0x00100000;          /* Carrega o kernel em 1 MB */

    .text ALIGN (0x1000) :   /* Alinhamento de 4 KB */
    {
        *(.text)             /* Código de todos os arquivos */
    }

    .rodata ALIGN (0x1000) : 
    {
        *(.rodata*)          /* Dados de apenas leitura (strings, constantes) */
    }

    .data ALIGN (0x1000) : 
    {
        *(.data)             /* Dados globais inicializados */
    }

    .bss ALIGN (0x1000) : 
    {
        *(.COMMON)           /* Variáveis globais não inicializadas */
        *(.bss)
    }
}
