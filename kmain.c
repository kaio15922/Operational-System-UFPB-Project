#include "framebuffer.h"
#include "serial.h"
#include "idt.h"
#include "gdt.h"
#include "multiboot.h"

// Tipo de "ponteiro para função sem argumentos e sem retorno". Vamos usar
// isso para transformar o endereço do módulo em algo que possa ser chamado
// como se fosse uma função C (Capítulo 7 - "Jumping to the Code").
typedef void (*call_module_t)(void);

/** run_first_module:
 *  Procura o primeiro módulo carregado pelo GRUB (o "program" definido em
 *  iso/modules) e pula para o seu código. Essa é a versão mais simples
 *  possível: o "programa" roda com os mesmos privilégios do kernel (modo
 *  kernel), já que ainda não temos paginação nem modo usuário.
 *
 *  @param ebx Endereço da estrutura multiboot_info passada pelo GRUB
 */
void run_first_module(unsigned int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

    // Sem essa flag, os campos mods_count/mods_addr nem sequer são válidos.
    if (!(mbinfo->flags & MULTIBOOT_FLAG_MODS)) {
        log_message(LOG_ERROR, "GRUB nao informou modulos (flag ausente).");
        return;
    }

    if (mbinfo->mods_count < 1) {
        log_message(LOG_ERROR, "Nenhum modulo foi carregado pelo GRUB.");
        return;
    }

    // mods_addr aponta para um array de module_t; pegamos só o primeiro.
    multiboot_module_t *modules = (multiboot_module_t *) mbinfo->mods_addr;
    unsigned int module_address = modules[0].mod_start;

    log_message(LOG_INFO, "Modulo encontrado! Pulando para o programa...");

    call_module_t start_program = (call_module_t) module_address;
    start_program();   // A partir daqui, quem executa é o "program.s"
    // Só voltamos para cá se o módulo, de alguma forma, retornar.
}

void kmain(unsigned int ebx) {
    //inicia a segmentacao
    init_gdt();

    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    idt_init();
    log_message(LOG_INFO, "Tabela IDT de 256 entradas carregada com sucesso.");

    char texto[] = "IDT e PIC Prontos! Pressione qualquer tecla...\n";
    fb_write(texto, sizeof(texto) - 1);

    // 1. ATIVA AS INTERRUPÇÕES NA CPU (Executa a instrução 'sti')
    // IMPORTANTE: isso precisa vir ANTES de pular para o módulo do Cap. 7,
    // porque run_first_module() pode nunca retornar (o "program.s" de teste
    // fica em loop infinito). Se o sti ficasse depois, o teclado nunca mais
    // seria atendido.
    __asm__ __volatile__("sti");

    // Capitulo 7: carrega e executa o modulo passado pelo GRUB.
    // Repare que o kernel "empresta" a CPU para o programa: como ainda nao
    // existe modo usuario, ele roda com o mesmo nivel de privilegio do
    // kernel. Mesmo que o programa fique em loop infinito (jmp $), como as
    // interrupções já estão ativas, o teclado continua funcionando.
    run_first_module(ebx);

    // 2. LOOP INFINITO: Mantém o kernel vivo.
    // A instrução 'hlt' faz o CPU dormir para poupar energia e acordar 
    // imediatamente quando o teclado disparar uma interrupção.
    while (1) {
        __asm__ __volatile__("hlt");
    }
}