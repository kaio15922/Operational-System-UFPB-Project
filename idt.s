global load_idt

; load_idt - Carrega a tabela descritora de interrupções (IDT).
; pilha (stack): [esp + 4] o endereço da estrutura idt_ptr
;                [esp    ] o endereço de retorno da função
load_idt:
    mov     eax, [esp + 4]    ; Pega o argumento (ponteiro passado pelo C)
    lidt    [eax]             ; Carrega a IDT apontada pelo registrador EAX
    ret                       ; Retorna para o C