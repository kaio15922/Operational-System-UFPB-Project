# Configurações dos Compiladores e Flags
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -c

AS = nasm
ASFLAGS = -f elf32

LD = ld
LDFLAGS = -T link.ld -melf_i386

# Lista de arquivos objetos que o sistema precisa para rodar
OBJECTS = loader.o io.o kmain.o framebuffer.o serial.o

# Regra principal (Roda quando você digita apenas 'make')
all: os.iso

# Como gerar a ISO final
os.iso: kernel.elf
	cp kernel.elf iso/boot/
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso

# Como linkar o Kernel
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# Como compilar os arquivos em C
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Como compilar os arquivos em Assembly
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Comando mágico para rodar tudo de primeira
run: os.iso
	bochs -f bochsrc.txt -q

# Comando para limpar a sujeira da pasta
clean:
	rm -f *.o kernel.elf os.iso bochsout.txt
