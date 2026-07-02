#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

// Estrutura de uma entrada na IDT (8 bytes / 64 bits)
struct idt_entry {
    unsigned short offset_low;        // Bits 0-15: Endereço baixo do Handler
    unsigned short segment_selector;  // Bits 16-31: Seletor de segmento de código (GDT, 0x08)
    unsigned char  reserved;          // Bits 32-39: Sempre zero
    unsigned char  type_attributes;   // Bits 40-47: Flags de atributos (ex: 0x8E)
    unsigned short offset_high;       // Bits 48-63: Endereço alto do Handler
} __attribute__((packed));            

// Estrutura do ponteiro da IDT (IDTR) que a CPU lê (6 bytes / 48 bits)
struct idt_ptr {
    unsigned short limit;             // Tamanho da tabela IDT menos 1
    unsigned int   base;              // Endereço de memória inicial da tabela idt[256]
} __attribute__((packed));

// Assinaturas das funções para serem usadas em outros arquivos (como kmain.c)
void idt_init(void);
void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags);

#endif /* INCLUDE_IDT_H */