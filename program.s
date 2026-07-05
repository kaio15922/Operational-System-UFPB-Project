; program.s
; Programa de teste minimo (Capitulo 7 do Little OS Book).
; Nao faz nada "util": apenas coloca um valor facil de reconhecer no
; registrador eax e entra em loop infinito. Isso serve para provarmos que o
; kernel conseguiu, de fato, encontrar o modulo carregado pelo GRUB e pular
; para o seu codigo.
;
; Compilado como BINARIO PLANO (flat binary), sem cabecalho ELF:
;   nasm -f bin program.s -o program
;
; IMPORTANTE: "nasm -f bin" assume 16 bits por padrão. Como nosso kernel já
; está rodando em modo protegido de 32 bits quando pula para cá, precisamos
; forçar o modo 32 bits explicitamente com [BITS 32], senão o nasm insere
; prefixos de operando (0x66) que fariam a CPU interpretar as instruções
; incorretamente em tempo de execução.
[BITS 32]

mov eax, 0xDEADBEEF   ; valor "marcador" para conferirmos depois no log do Bochs

.loop:
    jmp .loop         ; loop infinito - o programa nao tem mais nada a fazer