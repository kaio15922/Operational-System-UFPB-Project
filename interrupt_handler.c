#include "io.h"
#include "framebuffer.h" // Para podermos escrever na tela ao apertar a tecla

// 1. CORREÇÃO: Ordem exata dos bytes que a CPU Intel espera na IDT
struct idt_entry_struct {
    unsigned short offset_low;         // Bits 0-15
    unsigned short segment_selector;   // Bits 16-31
    unsigned char  reserved;           // Bits 32-39
    unsigned char  descriptor_attributes; // Bits 40-47
    unsigned short offset_high;        // Bits 48-63
} __attribute__((packed));

// 2. CORREÇÃO: Ordem invertida para bater com a sequência de PUSH do seu Assembly
struct cpu_state {
    unsigned int ebp; // Pushed por último (Endereço mais baixo)
    unsigned int edi;
    unsigned int esi;
    unsigned int edx;
    unsigned int ecx;
    unsigned int ebx;
    unsigned int eax; // Pushed primeiro (Endereço mais alto)
} __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   (PIC2_START_INTERRUPT + 7)
#define PIC_EOI 0x20

void pic_config() {
    // Sequência de inicialização padrão do PIC
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20); // Mapeia o PIC1 para começar em 0x20 (32)
    outb(PIC2_DATA, 0x28); // Mapeia o PIC2 para começar em 0x28 (40)

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Ativa todas as IRQs limpando as máscaras
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

void pic_acknowledge(unsigned int interrupt) {
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return;
    }

    if (interrupt < PIC2_START_INTERRUPT) {
        outb(PIC1_COMMAND, PIC_EOI);
    } else {
        outb(PIC2_COMMAND, PIC_EOI);
        outb(PIC1_COMMAND, PIC_EOI);
    }
}

extern unsigned char read_scan_code(void);

void interrupt_handler(struct cpu_state cpu, unsigned int interrupt, struct stack_state stack) {
    
    if (interrupt == 32) {
        // Vazio por enquanto
    }

    if (interrupt == 33) {
        // CORREÇÃO: Utiliza a tua função do keyboard.c
        unsigned char scancode = read_scan_code();

        // Se a tecla foi pressionada (bit 7 zerado)
        if ((scancode & 0x80) == 0) {
            fb_write("A", 1); 
        }
    }

    pic_acknowledge(interrupt);
}