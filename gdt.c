#include "gdt.h"

// Criamos o nosso array da GDT com 3 entradas (0 = Nula, 1 = Código, 2 = Dados)
gdt_entry_t gdt_entries[3];

// O ponteiro de 6 bytes que a CPU vai ler
gdt_ptr_t   gdt_ptr;

// Função que está lá no gdt.s (Assembly) para dar o 'flush' nos registradores
extern void gdt_flush(unsigned int gdt_ptr_addr);

/* Essa função auxiliar serve apenas para limpar a bagunça da Intel.
   Ela pega a Base e o Limite "normais" que passamos e espalha os bits 
   dentro daquela estrutura confusa de 8 bytes usando operações de Shift (>> e &).
*/
static void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    // Organiza a Base (onde começa) nos seus 3 pedaços
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    // Organiza o Limite (tamanho) nos seus 2 pedaços
    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    // Junta as flags de granularidade com o resto do limite e define o acesso
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

/* Função principal que monta a GDT e joga na CPU */
void init_gdt(void) {
    // Configura o ponteiro: o tamanho da tabela (3 entradas de 8 bytes = 24 bytes, menos 1 = 23)
    gdt_ptr.size    = (sizeof(gdt_entry_t) * 3) - 1;
    // Aponta o endereço para o nosso array gdt_entries
    gdt_ptr.address = (unsigned int)&gdt_entries;

    // Entrada 0: Registro Nulo. A CPU x86 exige que a primeira entrada seja toda ZERADA.
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entrada 1: Segmento de Código do Kernel (Offset / Seletor: 0x08)
    // Começa em 0, vai até 4GB (0xFFFFFFFF). 
    // 0x9A ativa: Segmento presente, Ring 0 (Kernel), executável e legível.
    // 0xCF ativa: Granularidade de 4KB (para chegar em 4GB) e modo 32-bits real.
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entrada 2: Segmento de Dados do Kernel (Offset / Seletor: 0x10)
    // Também cobre de 0 a 4GB, ficando "por cima" do segmento de código (Modelo Flat).
    // 0x92 ativa: Segmento presente, Ring 0 (Kernel), leitura e escrita (para variáveis e pilha).
    // 0xCF ativa: Mesma granularidade de 4KB e modo 32-bits.
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Passa o endereço do ponteiro para a nossa função em Assembly carregar no chip da CPU
    gdt_flush((unsigned int)&gdt_ptr);
}
