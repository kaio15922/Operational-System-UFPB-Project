#include "pmm.h"
#include "serial.h" // Para usarmos log_message e ver o que está acontecendo

#define PAGE_SIZE 4096
#define MAX_PAGES 32768 // Suportando até 128 MB de RAM. Aumente se quiser!

// O nosso famoso Bitmap. 1 bit = 1 Frame de 4KB.
static unsigned int memory_bitmap[MAX_PAGES / 32];
static unsigned int max_available_pages = 0;

// Funções internas auxiliares de manipulação de bits
static void bitmap_set(unsigned int bit) {
    memory_bitmap[bit / 32] |= (1 << (bit % 32));
}

static void bitmap_clear(unsigned int bit) {
    memory_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static int bitmap_test(unsigned int bit) {
    return memory_bitmap[bit / 32] & (1 << (bit % 32));
}

// Inicializa o Gerenciador de Memória
void pmm_init(multiboot_info_t *mbinfo, unsigned int kp_start, unsigned int kp_end) {
    // 1. Descobre quanta memória temos através do GRUB
    if (mbinfo->flags & MULTIBOOT_FLAG_MEM) {
        // mem_upper é dado em KB a partir de 1MB. 
        unsigned int total_kb = mbinfo->mem_upper + 1024; 
        max_available_pages = (total_kb * 1024) / PAGE_SIZE;
        
        if (max_available_pages > MAX_PAGES) {
            max_available_pages = MAX_PAGES; // Limita ao tamanho do nosso array
        }
    } else {
        log_message(LOG_ERROR, "GRUB nao passou o mapa de memoria!");
        return;
    }

    // 2. Zera tudo (Marca toda a memória física do PC como LIVRE)
    for (unsigned int i = 0; i < (MAX_PAGES / 32); i++) {
        memory_bitmap[i] = 0;
    }

    // 3. Reserva o 1º Megabyte inteiro (0x0 a 0x100000)
    // Motivo: O BIOS, Memória de Vídeo e o próprio GRUB vivem aqui.
    unsigned int first_mb_frames = 0x100000 / PAGE_SIZE;
    for (unsigned int i = 0; i < first_mb_frames; i++) {
        bitmap_set(i);
    }

    // 4. Reserva a área física que o nosso Kernel ocupa!
    unsigned int kernel_start_frame = kp_start / PAGE_SIZE;
    unsigned int kernel_end_frame = (kp_end / PAGE_SIZE) + 1; // +1 para garantir
    
    for (unsigned int i = kernel_start_frame; i <= kernel_end_frame; i++) {
        bitmap_set(i);
    }

    log_message(LOG_INFO, "Gerenciador de Memoria Fisica (PMM) Inicializado.");
}

// Aloca uma página livre
unsigned int pmm_alloc_page(void) {
    for (unsigned int i = 0; i < max_available_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);          // Marca como em uso
            return i * PAGE_SIZE;   // Retorna o endereço físico
        }
    }
    
    log_message(LOG_ERROR, "OUT OF MEMORY: Faltou memoria fisica!");
    return 0; 
}

// Libera uma página
void pmm_free_page(unsigned int physical_address) {
    unsigned int frame_index = physical_address / PAGE_SIZE;
    bitmap_clear(frame_index);
}
