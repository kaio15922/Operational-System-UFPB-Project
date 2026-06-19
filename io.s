global outb
global inb

; outb - Envia um byte (dado) para uma porta de hardware
; A pilha do C entrega: [esp+8] = dado, [esp+4] = porta
outb:
    mov al, [esp + 8]    ; O x86 exige que o DADO seja movido para o registo AL (8 bits)
    mov dx, [esp + 4]    ; O x86 exige que o endereço da PORTA seja movido para o registo DX (16 bits)
    out dx, al           ; Dispara o sinal elétrico (AL) para a porta (DX)
    ret                  ; Regressa à função em C

; inb - Lê um byte de uma porta de hardware
; A pilha do C entrega: [esp+4] = porta
inb:
    mov dx, [esp + 4]    ; Move o endereço da porta para o registo DX
    in  al, dx           ; Lê o estado da porta DX e guarda o resultado no registo AL
    ret                  ; O C assume automaticamente que o valor de retorno de uma função está em AL/EAX