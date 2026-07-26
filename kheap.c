#include "kheap.h"
#include "pmm.h"
#include "vmm.h"
#include "serial.h"

// O cabeçalho secreto que fica escondido antes de toda memória alocada
typedef struct block_header {
    unsigned int size;
    unsigned int is_free;
    struct block_header *next;
} block_header_t;

// Ponteiro para o início da nossa lista de memória
static block_header_t *heap_head = 0;

void kheap_init() {
    // 1. Pede 1 frame físico pro PMM (4 KB)
    unsigned int pagina_fisica = pmm_alloc_page();
    
    // 2. Mapeia para o endereço virtual mágico (0xC03FF000)
    heap_head = (block_header_t*) vmm_temp_map_page(pagina_fisica);
    
    // 3. Configura o primeiro bloco para ocupar a página inteira
    // Descontamos o tamanho da própria struct do cabeçalho
    heap_head->size = 4096 - sizeof(block_header_t);
    heap_head->is_free = 1;
    heap_head->next = 0;
    
    log_message(LOG_INFO, "Kernel Heap (malloc/free) inicializado em 0xC03FF000.");
}

void* kmalloc(unsigned int size) {
    block_header_t *current = heap_head;
    
    // Varre a lista procurando um bloco livre do tamanho pedido
    while (current != 0) {
        if (current->is_free == 1 && current->size >= size) {
            
            // O bloco serve! Vamos dividi-lo (Split) se sobrar espaço suficiente
            // Precisamos do tamanho pedido + tamanho de um novo cabeçalho + pelo menos 1 byte útil
            if (current->size > size + sizeof(block_header_t)) {
                
                // Cria um novo cabeçalho logo APÓS o espaço que vamos usar agora
                block_header_t *novo_bloco = (block_header_t*) ((unsigned int)current + sizeof(block_header_t) + size);
                
                novo_bloco->size = current->size - size - sizeof(block_header_t);
                novo_bloco->is_free = 1;
                novo_bloco->next = current->next;
                
                // Atualiza o bloco atual
                current->next = novo_bloco;
                current->size = size;
            }
            
            current->is_free = 0; // Marca como ocupado
            
            // Pula o cabeçalho e entrega o endereço exato dos dados
            return (void*) ((unsigned int)current + sizeof(block_header_t));
        }
        current = current->next;
    }
    
    log_message(LOG_ERROR, "OUT OF MEMORY NO HEAP: Nao ha blocos contiguos livres!");
    return 0; // NULL
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    // Anda para TRÁS na memória para ler o cabeçalho secreto
    block_header_t *header = (block_header_t*) ((unsigned int)ptr - sizeof(block_header_t));
    header->is_free = 1;
    
    // Algoritmo de Fusão (Coalescing):
    // Varre a lista toda. Se um bloco estiver livre E o próximo também estiver, junta os dois!
    block_header_t *current = heap_head;
    while (current != 0) {
        if (current->is_free == 1 && current->next != 0 && current->next->is_free == 1) {
            // Soma o tamanho do bloco atual + tamanho da struct do próximo + tamanho útil do próximo
            current->size = current->size + sizeof(block_header_t) + current->next->size;
            // Pula o próximo bloco na lista encadeada (ele foi absorvido)
            current->next = current->next->next;
        }
        current = current->next;
    }
}
