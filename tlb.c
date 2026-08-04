#include "tlb.h"

static tlb_entry_t tlb[TLB_SIZE];

static unsigned int proxima_posicao = 0;

void tlb_init(void)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb[i].numero_pagina = 0;
        tlb[i].numero_frame = 0;
        tlb[i].valido = 0;
    }
}

int tlb_lookup(unsigned char pagina)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].valido &&
            tlb[i].numero_pagina == pagina)
        {
            return tlb[i].numero_frame;
        }
    }

    return -1;
}

void tlb_insert(unsigned char pagina,
                unsigned char frame)
{
    tlb[proxima_posicao].numero_pagina = pagina;
    tlb[proxima_posicao].numero_frame = frame;
    tlb[proxima_posicao].valido = 1;

    proxima_posicao++;

    if (proxima_posicao >= TLB_SIZE)
    {
        proxima_posicao = 0;
    }
}