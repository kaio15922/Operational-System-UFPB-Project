#include "io.h"
#include "serial.h"

// Endereços de memória das portas da COM1
#define SERIAL_COM1_BASE                0x3F8
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/**
 * Configura o Baud Rate, Buffers e Modems da porta série.
 */
void serial_configure(unsigned short com) {
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x80); // Ativa o DLAB para configurar a velocidade
    outb(SERIAL_DATA_PORT(com), 0x03);         // Divisor baixo = 3 (38400 baud rate)
    outb(SERIAL_DATA_PORT(com), 0x00);         // Divisor alto = 0
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03); // Configura formato: 8 bits, sem paridade
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7); // Ativa e limpa as filas FIFO
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x0B); // Ativa os pinos DTR e RTS
}

/**
 * Verifica se a porta está livre para transmitir dados.
 */
int serial_is_transmit_fifo_empty(unsigned short com) {
    // Retorna verdadeiro se o bit 5 (0x20) do registo de status estiver a 1
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

/**
 * Envia um caractere pelo cabo.
 */
void serial_write_char(char c) {
    // Loop de espera (busy-wait) garantindo que não enviamos dados rápido demais
    while (serial_is_transmit_fifo_empty(SERIAL_COM1_BASE) == 0);
    outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
}

/**
 * Auxiliar para enviar arrays de caracteres (strings).
 */
void serial_write_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

/**
 * API de alto nível para envio de logs formatados.
 */
void log_message(int level, char *msg) {
    // Adiciona o prefixo correto na frente da mensagem para organizar o log
    if (level == LOG_INFO) {
        serial_write_string("[INFO] ");
    } else if (level == LOG_ERROR) {
        serial_write_string("[ERROR] ");
    } else if (level == LOG_DEBUG) {
        serial_write_string("[DEBUG] ");
    }
    
    serial_write_string(msg);
    serial_write_string("\n"); // Garante que a próxima mensagem fique na linha de baixo
}