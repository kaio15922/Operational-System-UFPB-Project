global gdt_flush
gdt_flush:
    mov eax, [esp + 4]  ; Pega o endereço do gdt_ptr que o C passou
    lgdt [eax]          ; Carrega a GDT usando a instrução da CPU

    ; Atualiza os registradores de segmento de dados para usar o seletor 0x10
    mov ax, 0x10        
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far Jump para atualizar o registrador de código (CS) para o seletor 0x08
    jmp 0x08:.flush
.flush:
    ret
