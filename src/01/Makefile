# DREL SDK v0.1 - Universal Makefile (Minimalist)
# Radi na Borland Make (Windows) i GNU Make (Linux)

CC = gcc
CFLAGS = -Wall -O2 -std=c99
TARGET = drel_vm

# Glavno pravilo | Main rule
all: drel_vm

# Pravilo za linkovanje | Linking rule
# Na Windowsu ce gcc automatski dodati .exe ako ga ne navedes
drel_vm: main.o drel_cpu.o drel_asm.o
	$(CC) $(CFLAGS) -o drel_vm main.o drel_cpu.o drel_asm.o

# Eksplicitna pravila za svaki objektni fajl
# Explicit rules for each object file
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

drel_cpu.o: drel_cpu.c
	$(CC) $(CFLAGS) -c drel_cpu.c

drel_asm.o: drel_asm.c
	$(CC) $(CFLAGS) -c drel_asm.c

# Ciscenje | Cleanup
clean:
	rm -f drel_vm drel_vm.exe main.o drel_cpu.o drel_asm.o bench.drel
