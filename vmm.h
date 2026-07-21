#ifndef VMM_H
#define VMM_H

// Pega um endereço FÍSICO e mapeia temporariamente na memória VIRTUAL
void* vmm_temp_map_page(unsigned int physical_address);

// Desfaz o mapeamento temporário (por segurança)
void vmm_temp_unmap_page(void);

#endif
