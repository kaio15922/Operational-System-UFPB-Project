global enter_user_mode

; Constantes
USER_MODE_CODE_SEGMENT_SELECTOR equ 0x18
USER_MODE_DATA_SEGMENT_SELECTOR equ 0x20
RPL                             equ 0x3

enter_user_mode:
    ; "The register ds, and the other data segment registers, should be set 
    ; to the same segment selector as ss."
    mov ax, (USER_MODE_DATA_SEGMENT_SELECTOR | RPL) ; 0x20 | 0x3 = 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; === PREPARANDO A PILHA PARA O IRET ===
    ; A pilha cresce para baixo, então empurramos na ordem inversa à da imagem.

    ; 1. [esp + 16] ss - stack segment selector para user mode
    push (USER_MODE_DATA_SEGMENT_SELECTOR | RPL) ; push 0x23

    ; 2. [esp + 12] esp - o ponteiro da pilha do user mode
    ; O texto indica: 0xBFFFFFFB (Nota: 0xC0000000 - 4 matematicamente seria 0xBFFFFFFC, 
    ; mas estamos usando exatamente o valor de exemplo que o autor forneceu)
    push 0xBFFFFFFB

    ; 3. [esp + 8] eflags - "For now, we should have interrupts disabled"
    ; Para garantir que as interrupções (flag IF) fiquem desabilitadas, 
    ; pegamos as flags atuais, zeramos o bit 9 e empurramos para a pilha.
    pushf
    pop eax
    and eax, 0xFFFFFDFF ; Máscara para zerar o bit de interrupção (IF)
    push eax

    ; 4. [esp + 4] cs - code segment selector para user mode
    push (USER_MODE_CODE_SEGMENT_SELECTOR | RPL) ; push 0x1B

    ; 5. [esp + 0] eip - o ponto de entrada do código de usuário
    ; O texto define o entry point como 0x00000000
    push 0x00000000

    ; "We are now ready to execute iret."
    iret