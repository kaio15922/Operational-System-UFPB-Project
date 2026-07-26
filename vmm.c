#include "vmm.h"
#include "pmm.h"
#include "serial.h"
#include "kheap.h"

// Importa a tabela e o diretório iniciais criados estaticamente no loader.s
extern pde_t boot_page_directory[];
extern pte_t boot_page_table[];

#define TEMP_PAGE_ADDRESS 0xC03FF000

// Macros para quebrar um endereço virtual nas coordenadas da MMU
#define PDE_INDEX(vaddr) ((vaddr) >> 22)
#define PTE_INDEX(vaddr) (((vaddr) >> 12) & 0x3FF)

// Limpa o cache TLB da CPU para um endereço específico
static void flush_tlb(unsigned int virtual_address) {
    __asm__ __volatile__("invlpg (%0)" ::"r"(virtual_address) : "memory");
}

// Inicialização básica (por enquanto aponta para o diretório padrão do boot)
void vmm_init(void) {
    log_message(LOG_INFO, "Gerenciador de Memoria Virtual (VMM) Dinamico Inicializado.");
}

page_directory_t* vmm_create_page_directory(void)
{
    // Reserva memória para o diretório
    page_directory_t *dir = (page_directory_t*)kmalloc(sizeof(page_directory_t));

    if (!dir)
        return 0;

    // Zera todas as entradas
    for (int i = 0; i < 1024; i++)
        dir->entries[i] = 0;

    /*
        Copia as entradas do Kernel.

        No Higher Half, o Kernel ocupa as entradas
        768 até 1023 do diretório.
    */
    for (int i = 768; i < 1024; i++)
        dir->entries[i] = boot_page_directory[i];

    return dir;
}

// Alterna o mapa de memória ativo na CPU (Usado para trocar de processo no Cap 11)
void vmm_switch_page_directory(page_directory_t *dir) {
    // A CPU precisa do endereço FÍSICO do diretório. Como o struct 'dir' estará 
    // no Higher-Half (C0000000+), nós subtraímos para achar o endereço físico real.
    unsigned int phys_addr = (unsigned int)dir - 0xC0000000;
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(phys_addr));
}

/**
 * vmm_map: A função que faltava no seu projeto!
 * Vincula dinamicamente um endereço virtual a uma página física na RAM.
 */
void vmm_map(page_directory_t *dir, unsigned int virtual_address, unsigned int physical_address, unsigned int flags) {
    unsigned int pde_idx = PDE_INDEX(virtual_address);
    unsigned int pte_idx = PTE_INDEX(virtual_address);

    // 1. Verifica se a Tabela de Páginas correspondente já existe no diretório
    if (!(dir->entries[pde_idx] & VMM_FLAG_PRESENT)) {
        // Se não existir, alocamos uma página física nova usando o seu PMM do Cap 10!
        unsigned int new_table_phys = pmm_alloc_page();
        
        // Registra a nova tabela no diretório com as permissões informadas
        dir->entries[pde_idx] = new_table_phys | VMM_FLAG_PRESENT | flags;
        
        // Limpa a tabela nova recém-alocada mapeando-a temporariamente
        pte_t *temp_table = (pte_t*)vmm_temp_map_page(new_table_phys);
        for(int i = 0; i < 1024; i++) temp_table[i] = 0;
        vmm_temp_unmap_page();
    }

    // 2. Localiza a tabela física para conseguir escrever a regra nela
    unsigned int table_phys_addr = dir->entries[pde_idx] & 0xFFFFF000;
    pte_t *table_virt = (pte_t*)vmm_temp_map_page(table_phys_addr);

    // 3. Grava a regra de tradução final na tabela
    table_virt[pte_idx] = (physical_address & 0xFFFFF000) | VMM_FLAG_PRESENT | flags;

    vmm_temp_unmap_page();
    flush_tlb(virtual_address); // Força a CPU a ler a nova regra
}

// Remove o mapeamento de um endereço virtual
void vmm_unmap(page_directory_t *dir, unsigned int virtual_address) {
    unsigned int pde_idx = PDE_INDEX(virtual_address);
    unsigned int pte_idx = PTE_INDEX(virtual_address);

    if (!(dir->entries[pde_idx] & VMM_FLAG_PRESENT)) return;

    unsigned int table_phys_addr = dir->entries[pde_idx] & 0xFFFFF000;
    pte_t *table_virt = (pte_t*)vmm_temp_map_page(table_phys_addr);

    table_virt[pte_idx] = 0; // Apaga a regra

    vmm_temp_unmap_page();
    flush_tlb(virtual_address);
}

void vmm_destroy_page_directory(page_directory_t *dir)
{
    if (!dir)
        return;

    /*
        Futuramente (capítulos seguintes)
        também iremos liberar todas as tabelas
        pertencentes ao processo.

        Por enquanto basta liberar o diretório.
    */

    kfree(dir);
}

// Mapeamento temporário (Mantido e corrigido para usar a tabela global do loader.s)
void* vmm_temp_map_page(unsigned int physical_address) {
    boot_page_table[1023] = (physical_address & 0xFFFFF000) | 0x03;
    flush_tlb(TEMP_PAGE_ADDRESS);
    return (void*) TEMP_PAGE_ADDRESS;
}

void vmm_temp_unmap_page(void) {
    boot_page_table[1023] = 0;
    flush_tlb(TEMP_PAGE_ADDRESS);
}
