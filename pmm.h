#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

// Inicializa o bitmap lendo a memória do GRUB e protegendo o Kernel
void pmm_init(multiboot_info_t *mbinfo, unsigned int kernel_physical_start, unsigned int kernel_physical_end);

// Encontra um frame (4KB) livre, marca como ocupado e retorna seu endereço FÍSICO
unsigned int pmm_alloc_page(void);

// Recebe um endereço FÍSICO e marca aquele frame como livre novamente no bitmap
void pmm_free_page(unsigned int physical_address);

#endif
