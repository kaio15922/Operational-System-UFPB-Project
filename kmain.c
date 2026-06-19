#include "framebuffer.h"
#include "serial.h"

void kmain() {
    // 1. Inicializa o hardware da porta série silenciosamente
    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    // 2. Cria uma mensagem e testa a lógica de escrita do ecrã
    char texto[] = "Vai tomar no cu JF\n";
    char texto2[] = "David eh gente boa\n";
    // Enviamos a string inteira, e a função cuidará do loop e do \n sozinha
    fb_write(texto, sizeof(texto) - 1);
    fb_write(texto2, sizeof(texto2) - 1);

    // 3. Imprime um log final confirmando o sucesso
    log_message(LOG_DEBUG, "O kernel concluiu a execucao com sucesso.");
}
