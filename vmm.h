#ifndef VMM_H
#define VMM_H

#define PAGE_SIZE 4096

// Flags de configuração dos bits inferiores das páginas (Seção 9.2 do livro)
#define VMM_FLAG_PRESENT  0x001  // Bit 0: A página está na RAM?
#define VMM_FLAG_WRITE    0x002  // Bit 1: Read-Write (1) ou Read-Only (0)
#define VMM_FLAG_USER     0x004  // Bit 2: Usuário/Ring 3 (1) ou Kernel/Ring 0 (0)

// Tipos base para representar as entradas da MMU x86
typedef unsigned int pde_t; // Page Directory Entry
typedef unsigned int pte_t; // Page Table Entry

// Estrutura que abstrai um Espaço de Endereçamento Virtual
typedef struct page_directory {
    pde_t entries[1024]; // Um diretório tem exatamente 1024 tabelas
} __attribute__((packed)) page_directory_t;

// Funções principais do Gerenciador de Memória Virtual (VMM)
void vmm_init(void);
page_directory_t* vmm_create_page_directory(void); // Cria um novo diretório de páginas
void vmm_switch_page_directory(page_directory_t *dir);
void vmm_map(page_directory_t *dir, unsigned int virtual_address, unsigned int physical_address, unsigned int flags);
void vmm_unmap(page_directory_t *dir, unsigned int virtual_address);
void vmm_destroy_page_directory(page_directory_t *dir); // Libera um diretório criado anteriormente


// Utilitários de mapeamento temporário (necessários para o PMM/VMM conversarem)
void* vmm_temp_map_page(unsigned int physical_address);
void vmm_temp_unmap_page(void);

#endif
