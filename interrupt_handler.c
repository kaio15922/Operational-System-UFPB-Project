#include "io.h"
#include "framebuffer.h" // Para podermos escrever na tela ao apertar a tecla

// --- DICIONÁRIO DO TECLADO (Scancode -> ASCII) ---
unsigned char teclado_qwerty[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',     /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 até F10 */
    0,  /* Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0, /* Tudo o resto é zero */
};

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
        // Rotina do Timer (Relógio do sistema). Vazio por enquanto.
    }

    if (interrupt == 33) {
        // Utiliza a sua função externa para ler a porta 0x60
        unsigned char scancode = read_scan_code();

        // Se a tecla foi pressionada (bit 7 zerado)
        if ((scancode & 0x80) == 0) {
            
            // Busca no dicionário qual é a letra correspondente a esse número
            char tecla_ascii = teclado_qwerty[scancode];

            // Se for uma tecla com representação visual (ignora Shift, Alt, etc)
            if (tecla_ascii != 0) {
                // Cria um array de 2 posições: a letra e o terminador nulo da string
                char str[2] = {tecla_ascii, '\0'};
                
                // Manda imprimir a string na tela!
                fb_write(str, 1); 
            }
        }
    }

    // Manda o "Aviso de Recebido" para o PIC liberar o teclado para a próxima tecla
    pic_acknowledge(interrupt);
}
