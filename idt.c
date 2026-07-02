#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr   idt_pointer;

extern void load_idt(unsigned int ptr_address);
extern void pic_config(void);

extern void interrupt_handler_0(void);
extern void interrupt_handler_1(void);
extern void interrupt_handler_7(void);

// 1. DECLARE O HANDLER DO TIMER AQUI
extern void interrupt_handler_32(void); 
extern void interrupt_handler_33(void); 

void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].offset_low = (base & 0xFFFF);
    idt[num].segment_selector = sel;
    idt[num].reserved = 0;
    idt[num].type_attributes = flags;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
}

void idt_init(void) {
    idt_pointer.limit = (sizeof(struct idt_entry) * 256) - 1;
    idt_pointer.base  = (unsigned int)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    pic_config();

    idt_set_gate(0,  (unsigned int)interrupt_handler_0,  0x08, 0x8E);
    idt_set_gate(1,  (unsigned int)interrupt_handler_1,  0x08, 0x8E);
    idt_set_gate(7,  (unsigned int)interrupt_handler_7,  0x08, 0x8E);
    
    // 2. REGISTRE O TIMER NA POSIÇÃO 32
    idt_set_gate(32, (unsigned int)interrupt_handler_32, 0x08, 0x8E);
    
    // O Teclado na posição 33
    idt_set_gate(33, (unsigned int)interrupt_handler_33, 0x08, 0x8E);

    load_idt((unsigned int)&idt_pointer);
}