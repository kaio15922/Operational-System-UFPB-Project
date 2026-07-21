#include "vmm.h"
#include "serial.h"

// Importamos a tabela de páginas direto do loader.s!
extern unsigned int boot_page_table[];

// Endereço virtual fixo para a gaveta 1023 (Sugerido pelo livro na seção 10.2)
#define TEMP_PAGE_ADDRESS 0xC03FF000

// Função interna para limpar o cache de endereços do Processador (TLB)
static void flush_tlb(unsigned int virtual_address) {
    __asm__ __volatile__("invlpg (%0)" ::"r"(virtual_address) : "memory");
}

void* vmm_temp_map_page(unsigned int physical_address) {
    // Pegamos o endereço físico puro (limpando o lixo dos 12 bits finais com &)
    // E somamos 0x03: Flag de Presente (1) + Leitura/Escrita (2)
    boot_page_table[1023] = (physical_address & 0xFFFFF000) | 0x03;

    // Avisa a CPU que a tabela mudou e ela precisa esquecer o endereço velho
    flush_tlb(TEMP_PAGE_ADDRESS);

    // Retorna o endereço VIRTUAL mágico onde agora você pode ler/escrever em C
    return (void*) TEMP_PAGE_ADDRESS;
}

void vmm_temp_unmap_page(void) {
    boot_page_table[1023] = 0; // Zera a entrada para evitar acessos acidentais
    flush_tlb(TEMP_PAGE_ADDRESS);
}
