global loader                   ; Símbolo de entrada para o Linker

MAGIC_NUMBER equ 0x1BADB002     ; Número mágico do Multiboot
FLAGS        equ 0x00000001     ; Alinha módulos na memória
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

KERNEL_STACK_SIZE equ 4096      ; Tamanho da Stack em bytes

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE      ; Reserva espaço para a pilha

section .text                   ; Voltando para a seção de código
loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE ; Aponta o ESP para o topo da pilha

    extern kmain
    call kmain                  ; Chama a função principal em C dos seus colegas

.loop:
    jmp .loop                   ; Loop de segurança caso o C retorne
