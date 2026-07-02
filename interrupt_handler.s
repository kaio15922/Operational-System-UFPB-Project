extern interrupt_handler

%macro no_error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword 0                     ; push de erro falso para manter o padrão na stack
        push    dword %1                    ; push do número da interrupção
        jmp     common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword %1                    ; o erro real já foi empilhado pelo processador, empilha só o número
        jmp     common_interrupt_handler
%endmacro

common_interrupt_handler:
    ; Salva os registradores exatamente na ordem reversa da struct cpu_state do C
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp

    call    interrupt_handler

    ; Restaura na ordem inversa
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    ; Limpa o número da interrupção e o código de erro que empilhamos nas macros
    add     esp, 8

    ; Retorna do tratamento de interrupção reativando as flags
    iret

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
error_code_interrupt_handler 7
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 33