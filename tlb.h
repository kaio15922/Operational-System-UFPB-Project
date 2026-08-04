#ifndef TLB_H
#define TLB_H

#define TLB_SIZE 16

typedef struct
{
    unsigned char numero_pagina;
    unsigned char numero_frame;
    unsigned char valido;

} tlb_entry_t;

void tlb_init(void);

int tlb_lookup(unsigned char pagina);

void tlb_insert(unsigned char pagina,
                unsigned char frame);

#endif