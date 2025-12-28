#pragma once
#ifndef DREL_CPU_H
#define DREL_CPU_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 1024 * 64 // 64KB

typedef struct {
    uint64_t regs[32];
    uint64_t pc;
    uint8_t* memory;
    bool running;
} DREL_CPU;

void cpu_init(DREL_CPU* cpu);
void cpu_step(DREL_CPU* cpu);
void cpu_dump_regs(DREL_CPU* cpu);
void cpu_free(DREL_CPU* cpu);

// Nova funkcija: Ucitava .drel binarni fajl sa diska
int cpu_load_bin_file(DREL_CPU* cpu, const char* filename);

#endif
