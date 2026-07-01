#ifndef GDT_H
#define GDT_H

/* Esta é a estrutura de UMA entrada (gaveta) da GDT. Ela tem exatamente 8 bytes.
   A Intel espalhou os bits da 'base' e do 'limit' por causa de processadores antigos.
   O '__attribute__((packed))' serve para o compilador (GCC) NÃO colocar espaços vazios
   de alinhamento na memória, mantendo os 8 bytes cravados.
*/
struct gdt_entry_struct {
    unsigned short limit_low;     // Os 16 bits iniciais do tamanho do segmento
    unsigned short base_low;      // Os 16 bits iniciais de onde o segmento começa na RAM
    unsigned char  base_middle;   // Mais 8 bits da base
    unsigned char  access;        // Byte de acesso: define se é Código/Dados e o privilégio (Ring)
    unsigned char  granularity;   // Flags de tamanho (se conta em bytes ou blocos de 4KB)
    unsigned char  base_high;     // Os últimos 8 bits da base
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

/* Este é o ponteiro especial que vamos passar para o comando 'lgdt' em Assembly.
   Ele tem exatamente 6 bytes e avisa a CPU onde a nossa tabela está na memória.
   ATENÇÃO: O 'size' (limite) vem ANTES do 'address' (base) porque a CPU x86 exige assim.
*/
struct gdt_ptr_struct {
    unsigned short size;        // Tamanho total da tabela GDT menos 1 (em bytes)
    unsigned int   address;     // Endereço de memória RAM onde o array da GDT começa
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

// Função que vamos chamar no kmain para ligar a GDT
void init_gdt(void);

#endif
