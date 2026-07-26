#ifndef KHEAP_H
#define KHEAP_H

// Inicializa o Kernel Heap pegando a página inicial do PMM/VMM
void kheap_init();

// Aloca 'size' bytes e retorna o ponteiro para uso
void* kmalloc(unsigned int size);

// Libera a memória previamente alocada e funde blocos vazios
void kfree(void* ptr);

#endif
