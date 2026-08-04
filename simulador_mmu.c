#include "simulador_dados.h"
#include "tlb.h"

#define PAGE_TABLE_SIZE 256

typedef struct
{
    unsigned char frame;
    unsigned char valido;

} page_table_entry_t;

static page_table_entry_t tabela_paginas[PAGE_TABLE_SIZE];

static unsigned int page_faults = 0;
static unsigned int tlb_hits = 0;

void mmu_init(void)
{
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        tabela_paginas[i].frame = 0;
        tabela_paginas[i].valido = 0;
    }
}

void traduzir_endereco(unsigned int endereco_virtual)
{
    unsigned char pagina;
    unsigned char offset;

    int frame;

    unsigned int endereco_fisico;

    pagina = (endereco_virtual >> 8) & 0xFF;
    offset = endereco_virtual & 0xFF;

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

            /*
             * A Pessoa 2 implementará:
             *
             * frame = pmm_alloc_page();
             *
             * memcpy(...);
             */

            tabela_paginas[pagina].frame = frame;
            tabela_paginas[pagina].valido = 1;
        }

        frame = tabela_paginas[pagina].frame;

        tlb_insert(pagina, frame);
    }

    endereco_fisico = (frame * PAGE_SIZE) + offset;

    /*
     * Ler o valor correspondente.
     */

    /*
     * printf(...);
     */
}