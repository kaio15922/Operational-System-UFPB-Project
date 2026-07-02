#include "framebuffer.h"
#include "serial.h"
#include "idt.h"
#include "gdt.h"

void kmain() {
    //inicia a segmentacao
    init_gdt();

    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    idt_init();
    log_message(LOG_INFO, "Tabela IDT de 256 entradas carregada com sucesso.");

    char texto[] = "IDT e PIC Prontos! Pressione qualquer tecla...\n";
    fb_write(texto, sizeof(texto) - 1);

    // 1. ATIVA AS INTERRUPÇÕES NA CPU (Executa a instrução 'sti')
    __asm__ __volatile__("sti");

    // 2. LOOP INFINITO: Mantém o kernel vivo.
    // A instrução 'hlt' faz o CPU dormir para poupar energia e acordar 
    // imediatamente quando o teclado disparar uma interrupção.
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
