#include "kheap.h"
#include "pmm.h"
#include "vmm.h"
#include "serial.h"

// Define o endereço virtual fixo e seguro para o Heap
#define KHEAP_START 0xC0400000

// Define quantas páginas o nosso Heap vai ter no início (64 páginas = 256 KB)
#define HEAP_INITIAL_PAGES 64

typedef struct block_header {
    unsigned int size;
    unsigned int is_free;
    struct block_header *next;
} block_header_t;

static block_header_t *heap_head = 0;

// Importa o diretório de páginas global para podermos mapear
extern unsigned int boot_page_directory[];

void kheap_init() {
    // 1. Pede frames físicos livres para começar (agora alocamos 64 páginas)
    for (int i = 0; i < HEAP_INITIAL_PAGES; i++) {
        unsigned int pagina_fisica = pmm_alloc_page();
        
        // 2. EM VEZ DE USAR O TEMP, MAPEIA DEFINITIVAMENTE NO NOVO VMM!
        // Usamos o boot_page_directory (ou o diretório atual do kernel)
        // Passamos as flags 0x03 (Presente + Escrita) para o Kernel rodar seguro
        vmm_map((page_directory_t*)boot_page_directory, KHEAP_START + (i * 4096), pagina_fisica, VMM_FLAG_WRITE);
    }
    
    // 3. O heap_head agora aponta para o endereço fixo seguro
    heap_head = (block_header_t*) KHEAP_START;
    
    // 4. Configura o bloco inicial com o tamanho expandido
    heap_head->size = (HEAP_INITIAL_PAGES * 4096) - sizeof(block_header_t);
    heap_head->is_free = 1;
    heap_head->next = 0;
    
    log_message(LOG_INFO, "Kernel Heap (malloc/free) expandido inicializado com seguranca em 0xC0400000.");
}

void* kmalloc(unsigned int size) {
    block_header_t *current = heap_head;
    
    while (current != 0) {
        if (current->is_free == 1 && current->size >= size) {
            if (current->size > size + sizeof(block_header_t) + 4) { // Margem de segurança de 4 bytes úteis
                block_header_t *novo_bloco = (block_header_t*) ((unsigned int)current + sizeof(block_header_t) + size);
                
                novo_bloco->size = current->size - size - sizeof(block_header_t);
                novo_bloco->is_free = 1;
                novo_bloco->next = current->next;
                
                current->next = novo_bloco;
                current->size = size;
            }
            
            current->is_free = 0;
            return (void*) ((unsigned int)current + sizeof(block_header_t));
        }
        current = current->next;
    }
    
    // Futuramente, em vez de dar erro direto aqui, chamar o pmm_alloc_page
    // e o vmm_map para aumentar o Heap dinamicamente (expandir além dos 4KB)!
    log_message(LOG_ERROR, "OUT OF MEMORY NO HEAP: Nao ha blocos contiguos livres!");
    return 0; 
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    block_header_t *header = (block_header_t*) ((unsigned int)ptr - sizeof(block_header_t));
    header->is_free = 1;
    
    // Algoritmo de Fusão (Coalescing) mantido perfeito
    block_header_t *current = heap_head;
    while (current != 0) {
        if (current->is_free == 1 && current->next != 0 && current->next->is_free == 1) {
            current->size = current->size + sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
            // Não avança o 'current' caso precise fundir com o próximo do próximo
            continue; 
        }
        current = current->next;
    }
}