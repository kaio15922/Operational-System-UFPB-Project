#include "io.h"
#include "framebuffer.h"

// Portas de I/O para controlar o cursor piscante do hardware
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

// Dimensões padrão do modo texto VGA
#define FB_COLS 80
#define FB_ROWS 25

// Variáveis globais para rastrear onde o cursor lógico está
unsigned int fb_col = 0;
unsigned int fb_row = 0;

/**
 * Escreve um único caractere na memória de vídeo com cor.
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    // Ponteiro direto para o endereço físico onde a placa de vídeo lê a tela
    char *fb = (char *) 0x000B8000;
    
    fb[i*2] = c; // O primeiro byte é o caractere da tabela ASCII
    
    // O segundo byte define as cores (4 bits para o fundo, 4 bits para a letra)
    // Usamos operadores bit-a-bit (Shift e OR) para fundir os dois valores num único byte
    fb[i * 2 + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

/**
 * Move o cursor físico (o tracinho branco piscante) da placa VGA.
 */
void fb_move_cursor(unsigned short pos) {
    // Como a posição pode ir até 2000, não cabe em 8 bits. 
    // Quebramos o número ao meio enviando os 8 bits altos e depois os 8 bits baixos.
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/**
 * Rola a tela uma linha para cima (Scroll) quando o monitor enche.
 */
void fb_scroll() {
    char *fb = (char *) 0x000B8000;
    
    // 1. Pega todas as linhas (da 1 até a 24) e copia para a linha de cima
    // Multiplicamos por 2 porque cada "célula" de tela gasta 2 bytes de memória
    for (unsigned int i = 0; i < FB_COLS * (FB_ROWS - 1) * 2; i++) {
        fb[i] = fb[i + (FB_COLS * 2)];
    }
    
    // 2. Limpa a última linha (linha 24) preenchendo com espaços vazios pretos
    unsigned int last_row_start = FB_COLS * (FB_ROWS - 1) * 2;
    for (unsigned int i = last_row_start; i < FB_COLS * FB_ROWS * 2; i += 2) {
        fb[i] = ' ';           // Espaço vazio
        fb[i + 1] = 0x0F;      // Fundo preto, letra branca
    }
    
    // 3. Volta o cursor para o início da última linha
    fb_row = FB_ROWS - 1;
    fb_col = 0;
}

/**
 * Função inteligente para escrever frases, processar o \n e gerir o scroll automático.
 */
void fb_write(char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        // Se detetar uma quebra de linha
        if (buf[i] == '\n') {
            fb_col = 0;
            fb_row++;
        } else {
            // Calcula a posição linear na memória baseada na linha e coluna atuais
            unsigned int pos = (fb_row * FB_COLS) + fb_col;
            
            // Pinta a letra (15 = Branco, 0 = Preto)
            fb_write_cell(pos, buf[i], 15, 0); 
            
            fb_col++;
            // Se chegou ao fim da largura da tela, desce para a próxima linha
            if (fb_col >= FB_COLS) {
                fb_col = 0;
                fb_row++;
            }
        }
        
        // Se o cursor tentou passar da última linha, aciona o Scroll
        if (fb_row >= FB_ROWS) {
            fb_scroll();
        }
    }
    
    // Após escrever tudo, atualiza a posição do cursor piscante do hardware
    fb_move_cursor((fb_row * FB_COLS) + fb_col);
}