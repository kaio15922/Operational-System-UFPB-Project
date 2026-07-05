#ifndef INCLUDE_MULTIBOOT_H
#define INCLUDE_MULTIBOOT_H

/* Versao minima da estrutura multiboot_info, contendo apenas os campos que
 * precisamos para o Capitulo 7 (encontrar o modulo carregado pelo GRUB).
 * A estrutura completa tem mais campos depois de mods_addr, mas como so
 * vamos ler os primeiros, nao precisamos declara-los todos.
 */
struct multiboot_info {
    unsigned int flags;       // Bit 3 (0x8) indica que mods_count/mods_addr sao validos
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;  // Quantidade de modulos carregados pelo GRUB
    unsigned int mods_addr;   // Endereco fisico do primeiro modulo (array de module_t)
} __attribute__((packed));

typedef struct multiboot_info multiboot_info_t;

/* Cada modulo carregado pelo GRUB é descrito por uma estrutura destas.
 * mods_addr aponta para um array de module_t (um por modulo carregado).
 */
struct multiboot_module {
    unsigned int mod_start;   // Endereco fisico de INICIO do modulo na memoria
    unsigned int mod_end;     // Endereco fisico de FIM do modulo na memoria
    unsigned int string;      // Ponteiro para uma string de identificacao (nao usamos)
    unsigned int reserved;
} __attribute__((packed));

typedef struct multiboot_module multiboot_module_t;

#define MULTIBOOT_FLAG_MODS 0x00000008

#endif /* INCLUDE_MULTIBOOT_H */