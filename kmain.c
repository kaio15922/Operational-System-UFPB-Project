#include "framebuffer.h"
#include "serial.h"
#include "idt.h"
#include "gdt.h"
#include "multiboot.h"
#include "pmm.h"
#include "vmm.h"

typedef void (*call_module_t)(void);

void run_first_module(unsigned int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

    if (!(mbinfo->flags & MULTIBOOT_FLAG_MODS)) {
        log_message(LOG_ERROR, "GRUB nao informou modulos (flag ausente).");
        return;
    }

    if (mbinfo->mods_count < 1) {
        log_message(LOG_ERROR, "Nenhum modulo foi carregado pelo GRUB.");
        return;
    }

    multiboot_module_t *modules = (multiboot_module_t *) (mbinfo->mods_addr + 0xC0000000);
    unsigned int module_virtual_address = modules[0].mod_start + 0xC0000000;

    log_message(LOG_INFO, "Modulo encontrado na memoria virtual! Pulando...");

    call_module_t start_program = (call_module_t) module_virtual_address;
    start_program();   
}

// Função auxiliar para converter um número inteiro para texto em Hexadecimal (ex: 0x00105000)
static void write_hex_log(char *mensagem, unsigned int valor) {
    char buffer[11] = "0x00000000"; // Molde inicial
    const char *hex_chars = "0123456789ABCDEF";
    
    // Converte de trás para frente usando deslocamento de bits
    unsigned int temp = valor;
    for (int i = 7; i >= 0; i--) {
        buffer[i + 2] = hex_chars[temp & 0xF];
        temp >>= 4;
    }
    
    // Imprime a mensagem descritiva e depois o número em Hex
    log_message(LOG_INFO, mensagem);
    log_message(LOG_INFO, buffer);
}

// O nosso teste do Capítulo 10
void testar_pmm() {
    log_message(LOG_INFO, "--- INICIANDO TESTE DO PMM E VMM ---");
    
    // 1. Pega uma página física VAZIA e CRUA
    unsigned int page_fisica = pmm_alloc_page();
    
    // 2. Mapeia ela no nosso endereço virtual mágico temporário
    char* page_virtual = (char*) vmm_temp_map_page(page_fisica);

    // 3. AGORA SIM! Podemos escrever na memória sem tomar Kernel Panic
    page_virtual[0] = 'O';
    page_virtual[1] = 'S';
    page_virtual[2] = '!';
    page_virtual[3] = '\0';

    log_message(LOG_INFO, "Consegui escrever na nova pagina física!");
    log_message(LOG_INFO, page_virtual); // Vai imprimir "OS!" no log do emulador
    
    // 4. Limpa a sujeira
    vmm_temp_unmap_page();
    pmm_free_page(page_fisica);
    
    log_message(LOG_INFO, "--- CAPITULO 10 CONCLUIDO ---");
}

/* --- INÍCIO DO CAPÍTULO 10 --- */
/* Assinatura modificada para receber os limites exportados pelo Linker via Assembly */
void kmain(unsigned int ebx, 
           unsigned int kv_start, 
           unsigned int kv_end, 
           unsigned int kp_start, 
           unsigned int kp_end) {
/* --- FIM DO CAPÍTULO 10 --- */

    init_gdt();

    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    // --- CAPÍTULO 10: Inicializa o PMM ---
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    pmm_init(mbinfo, kp_start, kp_end);
    // -------------------------------------
    testar_pmm();

    idt_init();
    log_message(LOG_INFO, "Tabela IDT de 256 entradas carregada com sucesso.");

    char texto[] = "IDT e PIC Prontos no Higher-Half! Pressione uma tecla...\n";
    fb_write(texto, sizeof(texto) - 1);

    __asm__ __volatile__("sti");

    run_first_module(ebx);

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
