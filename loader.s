global loader                   ; the entry symbol for ELF

extern kmain                    ; make the label kmain visible outside this file

KERNEL_STACK_SIZE equ 4096      ; Tamanho do stack em bytes

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
FLAGS        equ 0x0            ; multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                ; (magic number + checksum + flags should equal 0)

section .text:                  ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader:                         ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                ; stack (end of memory area)

    call kmain                                            

    .loop:
    jmp .loop                   ; loop forever

section .bss
align 4                         ; Alinha o stack em 4 bytes por performance
kernel_stack:                   ; Rótulo que aponta para o INÍCIO da área reservada
    resb KERNEL_STACK_SIZE      ; Reserva os 4096 bytes na memória