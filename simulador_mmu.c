#include "tlb.h"
#include "pmm.h"
#include "vmm.h"
#include "serial.h"

/* Declaracoes externas dos arrays definidos em simulador_dados.h.
 * Evita multiplas definicoes ao linkar (o kmain.c ja inclui o header). */
extern unsigned int  enderecos_teste[];
extern unsigned char BACKING_STORE_bin[];

#define PAGE_TABLE_SIZE 256
#define SIM_PAGE_SIZE   256  /* tamanho de pagina da simulacao (16 bits) */

typedef struct
{
    unsigned char frame;
    unsigned char valido;

} page_table_entry_t;

static page_table_entry_t tabela_paginas[PAGE_TABLE_SIZE];

static unsigned int page_faults = 0;
static unsigned int tlb_hits = 0;
static unsigned int total = 0;

/* Proximo frame fisico disponivel para a simulacao */
static unsigned char proximo_frame = 0;

/* Funcao auxiliar: imprime prefixo + numero no log serial.
 * Necessaria porque nao temos printf no kernel freestanding. */
static void log_numero(char *prefixo, unsigned int valor)
{
    char buf[12];
    int pos = 11;
    buf[--pos] = '\0'; // Terminador de string no final da array

    if (valor == 0) {
        log_message(LOG_INFO, prefixo);
        log_message(LOG_INFO, "0");
        return;
    }

    while (valor > 0 && pos > 0) {
        buf[--pos] = '0' + (valor % 10);
        valor /= 10;
    }

    log_message(LOG_INFO, prefixo);
    log_message(LOG_INFO, buf + pos);
}

void mmu_init(void)
{
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        tabela_paginas[i].frame = 0;
        tabela_paginas[i].valido = 0;
    }

    tlb_init();
}

void traduzir_endereco(unsigned int endereco_virtual)
{
    unsigned char pagina;
    unsigned char offset;

    int frame;

    unsigned int endereco_fisico;

    pagina = (endereco_virtual >> 8) & 0xFF;
    offset = endereco_virtual & 0xFF;

    total++;

    frame = tlb_lookup(pagina);

    if (frame != -1)
    {
        tlb_hits++;
    }
    else
    {
        if (!tabela_paginas[pagina].valido)
        {
            page_faults++;

            /* 1. Usa o PMM para alocar 4KB da memoria RAM REAL da maquina! */
            unsigned int frame_fisico_real = pmm_alloc_page();

            /* 2. O processador proibe escrever em enderecos fisicos crus. 
             * Usamos o VMM para criar uma janela virtual temporaria para essa RAM. */
            char* janela_virtual = (char*) vmm_temp_map_page(frame_fisico_real);

            /* 3. Copia os 256 bytes do "Disco" (BACKING_STORE) para a RAM REAL Mapeada */
            unsigned int src = (unsigned int)pagina * SIM_PAGE_SIZE;
            for (unsigned int b = 0; b < SIM_PAGE_SIZE; b++) {
                janela_virtual[b] = BACKING_STORE_bin[src + b];
            }

            /* 4. Fecha a janela para manter a seguranca do Kernel */
            vmm_temp_unmap_page();

            /* 5. A simulacao original do livro espera frames logicos de 0 a 255
             * entao mantemos o contador logico para as estatisticas baterem. */
            frame = proximo_frame++;

            tabela_paginas[pagina].frame = (unsigned char) frame;
            tabela_paginas[pagina].valido = 1;
        }

        frame = tabela_paginas[pagina].frame;

        tlb_insert(pagina, frame);
    }

    endereco_fisico = ((unsigned int)frame * SIM_PAGE_SIZE) + offset;

    /* Le o valor no endereco fisico do BACKING_STORE —
     * e o "valor" que o Silberschatz pede imprimir no gabarito. */
    unsigned char valor_lido = BACKING_STORE_bin[
        ((unsigned int)pagina * SIM_PAGE_SIZE) + offset
    ];

    /* Imprime no formato exigido pelo livro (via porta serial -> com1.out):
     *   Virtual: XXXXX  Fisico: YYYYY  Valor: ZZZ */
    log_numero("Virtual: ", endereco_virtual);
    log_numero("Fisico:  ", endereco_fisico);
    log_numero("Valor:   ", (unsigned int)valor_lido);
}

/* Imprime as estatisticas finais — os dois numeros do gabarito do professor:
 *   Page Faults: ~244 (~24%)
 *   TLB Hits:    ~ 54 (~ 5%) */
void mmu_imprimir_estatisticas(void)
{
    unsigned int pct_faults = (page_faults * 100) / total;
    unsigned int pct_hits   = (tlb_hits   * 100) / total;

    log_message(LOG_INFO, "=== ESTATISTICAS FINAIS DO SIMULADOR ===");
    log_numero("Total de enderecos:      ", total);
    log_numero("Page Faults:             ", page_faults);
    log_numero("TLB Hits:                ", tlb_hits);
    log_numero("Taxa de Page Faults (%): ", pct_faults);
    log_numero("Taxa de TLB Hits    (%): ", pct_hits);
    log_message(LOG_INFO, "Gabarito: ~244 page faults (~24%) e ~54 TLB hits (~5%)");
    log_message(LOG_INFO, "========================================");
}