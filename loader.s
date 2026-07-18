global loader                   ; Símbolo de entrada para o Linker

MAGIC_NUMBER equ 0x1BADB002     ; Número mágico do Multiboot
FLAGS        equ 0x00000001     ; Alinha módulos na memória
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

; --- MÁGICA DO HIGHER-HALF ---
; Macro para calcular o endereço físico na RAM subtraindo 3 GB (0xC0000000).
; Usaremos isso nas primeiras linhas, enquanto a Paginação ainda estiver desligada.
%define PHY(addr) (addr - 0xC0000000)

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
    ; -------------------------------------------------------------------------
    ; 1. PREENCHENDO A TABELA DE PÁGINAS (Mapeia 1024 entradas = 4 MB físicos)
    ; Como a paginação está desligada, usamos os endereços FÍSICOS da memória
    ; -------------------------------------------------------------------------
    mov edi, PHY(boot_page_table)
    mov esi, 0
    mov ecx, 1024

.map_loop:
    mov edx, esi
    or edx, 0x00000003          ; Flags: Presente (1) e Leitura/Escrita (2)
    mov [edi], edx              ; Grava a entrada na tabela
    add esi, 4096               ; Próxima página física de 4KB
    add edi, 4                  ; Próxima posição de 4 bytes no array
    loop .map_loop

    ; -------------------------------------------------------------------------
    ; 2. O MAPEAMENTO IDÊNTICO E O HIGHER-HALF
    ; -------------------------------------------------------------------------
    ; Colocamos a tabela na gaveta 0 do diretório (Mapeamento Idêntico de 0 a 4MB)
    ; Isso garante que o código não capote no exato segundo em que ativarmos a chave.
    mov dword [PHY(boot_page_directory) + 0], PHY(boot_page_table) + 0x003

    ; 3 GB (0xC0000000) divididos por 4MB = Gaveta 768. 
    ; Gaveta 768 * 4 bytes de tamanho = offset 3072 no array.
    mov dword [PHY(boot_page_directory) + 3072], PHY(boot_page_table) + 0x003

    ; -------------------------------------------------------------------------
    ; 3. LIGANDO A PAGINAÇÃO
    ; -------------------------------------------------------------------------
    mov eax, PHY(boot_page_directory)
    mov cr3, eax                ; Entrega o endereço do mapa para a CPU

    mov eax, cr0
    or eax, 0x80000000          ; Liga o bit 31 (PG - Paging) do registrador mestre
    mov cr0, eax                ; A Paginação está ATIVA!

    ; -------------------------------------------------------------------------
    ; 4. O SALTO DE FÉ PARA OS 3 GB
    ; -------------------------------------------------------------------------
    ; 'lea' pega o endereço virtual (3GB+) que o Linker script (.ld) gerou
    lea eax, [higher_half]
    jmp eax                     ; Salto final!

higher_half:
    ; --- BEM-VINDO AO MUNDO VIRTUAL DOS 3 GB ---
    
    ; Desfaz o Mapeamento Idêntico (Nenhum código do kernel deve usar a memória baixa)
    mov dword [boot_page_directory + 0], 0
    invlpg [0]                  ; Limpa o cache de tradução da CPU

    ; Agora que a memória está mapeada e estável nos 3GB, armamos a pilha
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    ; O GRUB deixou no `ebx` o endereço físico da struct multiboot. 
    ; Para o nosso C conseguir ler ele sem dar Page Fault, somamos 3 GB nele aqui mesmo:
    add ebx, 0xC0000000
    push ebx                    ; Envia o endereço virtualizado para a kmain

    extern kmain
    call kmain                  ; Chama a função principal em C

.loop:
    hlt                         ; Dorme o processador até vir a próxima interrupção
    jmp .loop                   ; Loop de segurança
