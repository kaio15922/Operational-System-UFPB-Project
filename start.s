extern kmain

section .text
    ; push argv (pode ser implementado no futuro)
    ; push argc (pode ser implementado no futuro)
    call kmain
    ; quando kmain retornar, eax terá o valor de retorno
    jmp $       ; loop infinito