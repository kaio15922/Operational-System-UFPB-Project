#ifndef SIMULADOR_MMU_H
#define SIMULADOR_MMU_H

#include "simulador_dados.h"   /* expoe enderecos_teste[] e NUM_ENDERECOS pro kmain */

/* Passo 3 - Missão da Pessoa 2
 *
 * Interface pública do simulador de gerenciamento de memória virtual,
 * baseado no programming project do Capítulo 10 do Silberschatz
 * (Operating System Concepts, 10a ed).
 *
 * O kmain.c inclui apenas este header — não precisa conhecer os detalhes
 * internos do simulador (tabela de páginas, contadores, etc.).
 */

/* Inicializa o simulador: zera a tabela de páginas e a TLB.
 * DEVE ser chamado depois de kheap_init() no kmain. */
void mmu_init(void);

/* Traduz um endereço virtual de 16 bits para físico, consultando
 * TLB → tabela de páginas → backing store (page fault se necessário),
 * e imprime o resultado no log serial. */
void traduzir_endereco(unsigned int endereco_virtual);

/* Imprime as estatísticas finais (page faults e TLB hits) no log serial.
 * Chamar depois do loop de 1000 endereços para ver o gabarito. */
void mmu_imprimir_estatisticas(void);

#endif /* SIMULADOR_MMU_H */