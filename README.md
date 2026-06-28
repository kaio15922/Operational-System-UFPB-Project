# HydrogenSO

## Sobre o Projeto

### Objetivo

**O Projeto ainda está em desenvolvimento...**

Desenvolver um sistema operacional educacional básico de 32 bits (arquitetura x86) baseado nos conceitos do The Little OS Book, aplicando na prática conceitos de baixo nível, integração entre Assembly e C, e gerenciamento de hardware.

### Descrição Geral

O HydrogenSO é um kernel simples projetado para rodar em modo protegido de 32 bits. O projeto demonstra a inicialização de um sistema computacional a partir de um bootloader padrão (Multiboot), estabelecendo um ambiente em C para execução de código de controle de hardware, saída de vídeo elementar.

### Principais Funcionalidades

(Em desenvolvimento)

## Interface do Sistema (Em desenvolvimento)

### Captura de Tela da Interface

### Descrição da Interface

---

## Como Compilar e Executar

### Pré-requisitos

- Sistema Operacional baseado em Linux (preferencialmente distribuições derivadas do Debian/Ubuntu).

- Ferramentas essenciais de compilação (build-essential).

### Dependências

Para montar o ambiente de compilação cruzada de 32 bits e emulação, instale os pacotes necessários rodando:

```
sudo apt update && sudo apt install build-essential nasm genisoimage bochs bochs-sdl seabios vgabios -y
```

OBS.: O bochs funciona melhor quando usado em uma Máquina Virtual. Caso o bochs não funcione na sua máquina, fique a vontade na utilização do QEMU.

### Instalação

- git clone https://github.com/kaio15922/Operational-System-UFPB-Project.git
- cd Operational-System-UFPB-Project

### Compilação

Todo o processo de compilação de arquivos .c e .s, linkagem ELF de 32 bits e criação da imagem de disco estruturada com o GRUB é automatizado pelo Makefile.

### Execução

Para limpar builds anteriores, compilar os fontes atuais e disparar o emulador Bochs automaticamente, execute o comando:

```
make run
```

OBS.: Quando a janela do Bochs se iniciar (ficando inicialmente preta), volte ao terminal do Linux, digite c (continue) e aperte Enter para iniciar o boot do sistema.

---

## Como Utilizar

### Inicialização

O sistema é carregado automaticamente pelo arquivo os.iso gerado na pasta raiz. O GRUB localiza o cabeçalho Multiboot dentro do binário do kernel, valida o checksum e passa o controle do processador para o entrypoint em Assembly.

### Comandos Disponíveis

(Em desenvolvimento)

### Exemplos de Uso

Ao ligar, o kernel executa as rotinas internas do arquivo kmain.c, escrevendo mensagens de teste diretamente na tela do emulador através do driver do framebuffer e registrando eventos internos (como informações de inicialização e depuração) de forma silenciosa dentro do arquivo local com1.out.

OBS.: Você mesmo pode mexer nisso. Dentro do repositório, digite **nano kmain.c** e fique a vontade para modificar o arquivo. Não se esqueça de salvar as alterações.

---

## Estrutura do Projeto

### Organização dos Diretórios
```
HydrogenSO/
├── iso/
│   └── boot/
│       └── grub/
│           ├── menu.lst          # Configuração de inicialização do GRUB
│           └── stage2_eltorito   # Binário de boot do GRUB Legacy
├── Makefile                      # Script de automação do build
├── bochsrc.txt                   # Arquivo de configuração do emulador Bochs
└── *.[c|h|s]                     # Arquivos de código-fonte do Kernel
```
### Arquivos Principais
- loader.s: Código em Assembly x86 responsável por declarar as flags mágicas do Multiboot, alocar a stack do kernel na memória e desviar o fluxo para a rotina em C.

- kmain.c: Ponto de entrada da lógica em C, onde são inicializados os componentes de hardware e disparadas as escritas de texto.

- framebuffer.c / framebuffer.h: Implementação das funções de manipulação de cursores e envio de bytes para a memória de vídeo.

- serial.c / serial.h: Lógica de controle e envio de dados para as portas de E/S seriais.

- io.s / io.h: Funções em Assembly (outb, inb) necessárias para conversar diretamente com as portas físicas da CPU, indisponíveis nativamente no C puro.

- link.ld: Linker script utilizado para ordenar as seções de memória (.text, .rodata, .data, .bss) respeitando o alinhamento exigido pelo padrão Multiboot.
---

## Equipe e Contribuições

### Integrantes

Kaio Renato de Melo Marinho  | Matrícula: 20240009222
David Alves Norberto         | Matrícula: 20240009160
João Filipe de Souza Rosa    | Matrícula: 20240009189

### Divisão de Tarefas

#### Membro 1 (Kaio)

Responsável pelo Capítulo 2, estruturando o repositório Git e a infraestrutura base do projeto. Atuou também na correção de bugs do Capítulo 4, realizando ajustes das seções de boot no loader.s, ajustes de flags e caminhos no Makefile para garantir o build estrito de 32 bits e a resolução de conflitos de BIOS no emulador Bochs.

(Mais em breve)

#### Membro 2 (David)

Responsável pelo Capítulo 4, desenvolvendo a lógica de controle dos componentes físicos. Implementou o driver de Framebuffer para escrita na memória de vídeo VGA (0x000B8000), configurou a Porta Serial (COM1) usando funções de E/S em Assembly (io.s) e estruturou o Makefile original do projeto para automatizar as etapas de compilação e linkagem.

(Mais em breve)

#### Membro 3 (João)

Responsável pelo Capítulo 3, implementando a rotina principal do kernel (kmain.c) e estabelecendo a transição de execução do baixo nível para o alto nível. Atuou na estruturação do linker script (link.ld) e no ordenamento das seções de memória (.text, .data, .bss) exigidas para o mapeamento correto do executável do sistema.

(Mais em breve)
