global loader                   ; Símbolo de entrada para o Linker
global boot_page_table          ; Exporta a tabela para o C

MAGIC_NUMBER equ 0x1BADB002     ; Número mágico do Multiboot
FLAGS        equ 0x00000001     ; Alinha módulos na memória
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

; --- MÁGICA DO HIGHER-HALF ---
%define PHY(addr) (addr - 0xC0000000)

; --- INÍCIO DO CAPÍTULO 10 ---
; Importa as variáveis geradas pelo Linker script (.ld)
extern kernel_virtual_start
extern kernel_virtual_end
extern kernel_physical_start
extern kernel_physical_end
; --- FIM DO CAPÍTULO 10 ---

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

KERNEL_STACK_SIZE equ 4096      ; Tamanho da Stack em bytes

section .bss
align 4096                      ; O hardware EXIGE que o mapa de páginas seja múltiplo de 4KB
boot_page_directory:
    resb 4096                   ; Reserva o Diretório mestre (4 KB)
boot_page_table:
    resb 4096                   ; Reserva a primeira Tabela de páginas (4 KB)

align 4
kernel_stack:
    resb KERNEL_STACK_SIZE      ; Reserva espaço para a pilha

section .text                   ; Voltando para a seção de código
loader:
    ; [SUA LÓGICA DE PAGINAÇÃO INTACTA...]
    mov edi, PHY(boot_page_table)
    mov esi, 0
    mov ecx, 1024

.map_loop:
    mov edx, esi
    or edx, 0x00000003          
    mov [edi], edx              
    add esi, 4096               
    add edi, 4                  
    loop .map_loop

    mov dword [PHY(boot_page_directory) + 0], PHY(boot_page_table) + 0x003
    mov dword [PHY(boot_page_directory) + 3072], PHY(boot_page_table) + 0x003

    mov eax, PHY(boot_page_directory)
    mov cr3, eax                

    mov eax, cr0
    or eax, 0x80000000          
    mov cr0, eax                

    lea eax, [higher_half]
    jmp eax                     

higher_half:
    mov dword [boot_page_directory + 0], 0
    invlpg [0]                  

    mov esp, kernel_stack + KERNEL_STACK_SIZE

    ; O GRUB deixou no `ebx` o endereço físico da struct multiboot. 
    add ebx, 0xC0000000
    
    ; --- INÍCIO DO CAPÍTULO 10 ---
    ; Passando argumentos para o C. Pela convenção x86 cdecl, 
    ; os argumentos são empurrados na ordem INVERSA da assinatura da função.
    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start
    push ebx                    ; Deixa o ebx como o PRIMEIRO argumento
    ; --- FIM DO CAPÍTULO 10 ---

    extern kmain
    call kmain                  

.loop:
    hlt                         
    jmp .loop
