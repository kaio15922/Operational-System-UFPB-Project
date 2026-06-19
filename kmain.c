#include "framebuffer.h"
#include "serial.h"

void kmain() {
    // 1. Inicializa o hardware da porta série silenciosamente
    serial_configure(SERIAL_COM1_BASE);
    log_message(LOG_INFO, "Sistema Iniciado! Porta Serie configurada.");

    // 2. Cria uma mensagem e testa a lógica de escrita do ecrã
    char texto[] = "Bem-vindo ao HydrogenSO!\nIsto e um teste de sistema do Capitulo 4.\n";
    
    // Enviamos a string inteira, e a função cuidará do loop e do \n sozinha
    write(texto, sizeof(texto) - 1);

    // 3. Imprime um log final confirmando o sucesso
    log_message(LOG_DEBUG, "O kernel concluiu a execucao com sucesso.");
}