#include "framebuffer.h"
#include "serial.h"

void kmain() {
    // Inicializa o hardware da porta serial
    // Imprime um log de info
    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    // Cria uma mensagem e testa a lógica de escrita do ecrã
    char texto[] = "Printando na tela com sucesso.\n";
    char texto2[] = "Gears of War melhor que God of War.\n";
    char texto3[] = "Cap 4 feito!\n";
    // Enviamos a string inteira, e a função cuidará do loop e do \n sozinha
    fb_write(texto, sizeof(texto) - 1);
    fb_write(texto2, sizeof(texto2) - 1);
    fb_write(texto3, sizeof(texto3) - 1);

    // Imprime um log (debug) final confirmando o sucesso
    log_message(LOG_DEBUG, "O kernel concluiu a execucao com sucesso.");
}
