# Configurações dos Compiladores e Flags
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -c

AS = nasm
ASFLAGS = -f elf32

LD = ld
LDFLAGS = -T link.ld -melf_i386

# Lista de arquivos objetos que o sistema precisa para rodar
OBJECTS = loader.o io.o kmain.o framebuffer.o serial.o gdt.o gdt_asm.o idt.o idt_asm.o interrupt_handler.o interrupt_handler_asm.o keyboard.o

# Regra principal (Roda quando você digita apenas 'make')
all: os.iso

# Como gerar a ISO final (agora também depende do programa de teste do Cap. 7)
os.iso: kernel.elf program
	cp kernel.elf iso/boot/
	mkdir -p iso/modules
	cp program iso/modules/program
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso

# Programa de teste do Capítulo 7 ("The Road to User Mode").
# Compilado como binário plano (flat binary), sem cabeçalho ELF, pois nosso
# kernel ainda não sabe interpretar esse formato.
program: program.s
	$(AS) -f bin program.s -o program

# Como linkar o Kernel
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# Regra específica para o gdt.c
gdt.o: gdt.c gdt.h
	$(CC) $(CFLAGS) -c gdt.c -o gdt.o

# Regra específica para o gdt.s (compila como gdt_asm.o para não atropelar o gdt.c)
gdt_asm.o: gdt.s
	$(AS) $(ASFLAGS) gdt.s -o gdt_asm.o

idt_asm.o: idt.s
	$(AS) $(ASFLAGS) idt.s -o idt_asm.o

interrupt_handler_asm.o: interrupt_handler.s
	$(AS) $(ASFLAGS) interrupt_handler.s -o interrupt_handler_asm.o

# Como compilar os arquivos em C
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Como compilar os arquivos em Assembly
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Caso seja QEMU, substitua por: qemu-system-i386 -cdrom os.iso
run: os.iso
	bochs -f bochsrc.txt -q

# Comando para limpar a sujeira da pasta
clean:
	rm -f *.o kernel.elf os.iso bochsout.txt program iso/modules/program