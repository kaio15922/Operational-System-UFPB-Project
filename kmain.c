#include "framebuffer.h"
#include "serial.h"
#include "idt.h"
#include "gdt.h"
#include "multiboot.h"

// Tipo de "ponteiro para função sem argumentos e sem retorno".
typedef void (*call_module_t)(void);

/** run_first_module:
 * Procura o primeiro módulo carregado pelo GRUB e pula para o seu código.
 * Agora com suporte a paginação: os ponteiros físicos do GRUB são convertidos
 * para endereços virtuais correspondentes na metade superior (Higher-Half).
 *
 * @param ebx Endereço virtualizado da estrutura multiboot_info
 */
void run_first_module(unsigned int ebx) {
    // O ebx já chega aqui convertido para o endereço virtual pelo loader.s
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

    /* CORREÇÃO PARA O CAPÍTULO 9:
     * O campo mbinfo->mods_addr contém o endereço FÍSICO do array de módulos.
     * Como a memória física baixa está desmapeada, precisamos somar 0xC0000000 (3 GB)
     * para obter o endereço virtual equivalente e permitir a leitura estável pelo C.
     */
    multiboot_module_t *modules = (multiboot_module_t *) (mbinfo->mods_addr + 0xC0000000);
    
    /* CORREÇÃO PARA O CAPÍTULO 9:
     * O campo mod_start armazena o endereço FÍSICO onde o program.s foi descarregado.
     * Somamos 0xC0000000 para que o salto da CPU seja feito dentro do espaço virtual.
     */
    unsigned int module_virtual_address = modules[0].mod_start + 0xC0000000;

    log_message(LOG_INFO, "Modulo encontrado na memoria virtual! Pulando...");

    call_module_t start_program = (call_module_t) module_virtual_address;
    start_program();   // Executa o "program.s" mapeado em 3 GB
}

void kmain(unsigned int ebx) {
    // Inicia a segmentação global
    init_gdt();

    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    idt_init();
    log_message(LOG_INFO, "Tabela IDT de 256 entradas carregada com sucesso.");

    char texto[] = "IDT e PIC Prontos no Higher-Half! Pressione uma tecla...\n";
    fb_write(texto, sizeof(texto) - 1);

    // 1. ATIVA AS INTERRUPÇÕES NA CPU (Executa a instrução 'sti')
    __asm__ __volatile__("sti");

    // Capítulo 9: Executa o módulo passando o ebx que já foi previamente 
    // ajustado no loader.s para refletir o endereço virtual correspondente.
    run_first_module(ebx);

    // 2. LOOP INFINITO: Mantém o kernel vivo.
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
