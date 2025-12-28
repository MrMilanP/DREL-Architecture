#include "drel_cpu.h"
#include "drel_isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cpu_init(DREL_CPU* cpu) {
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->pc = 0;
    cpu->memory = (uint8_t*)malloc(MEMORY_SIZE);
    memset(cpu->memory, 0, MEMORY_SIZE);
    cpu->running = true;
}

int cpu_load_bin_file(DREL_CPU* cpu, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("[CPU Error] Cannot open file: %s\n", filename);
        return 0;
    }

    // Ucitaj fajl direktno u memoriju procesora
    // Load file directly into processor memory
    size_t bytes_read = fread(cpu->memory, 1, MEMORY_SIZE, f);
    fclose(f);

    printf("[CPU] Loaded binary '%s' (%zu bytes).\n", filename, bytes_read);
    return 1;
}

void cpu_step(DREL_CPU* cpu) {
    if (!cpu->running) return;

    // FETCH - Dohvatanje instrukcije
    // FETCH - Instruction fetching
    uint32_t instr = *(uint32_t*)&cpu->memory[cpu->pc];
    cpu->pc += 4;

    // DECODE - Dekodiranje polja
    // DECODE - Field decoding
    uint8_t opcode = (instr >> 26) & 0x3F;
    uint8_t rd = (instr >> 21) & 0x1F;
    uint8_t rs1 = (instr >> 16) & 0x1F;
    uint8_t rs2 = (instr >> 11) & 0x1F;
    int16_t imm = (int16_t)(instr & 0xFFFF);

    cpu->regs[0] = 0; // R0 je uvek nula | R0 is always zero

    // EXECUTE - Izvršavanje instrukcije
    // EXECUTE - Instruction execution
    switch (opcode) {
    case OP_EXIT:
        printf("[EXEC] EXIT at PC=%lld\n", cpu->pc - 4);
        cpu->running = false;
        break;
    case OP_ADDI:
        cpu->regs[rd] = cpu->regs[rs1] + imm;
        break;
    case OP_ADD:
        cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
        break;
    case OP_LI:
        cpu->regs[rd] = imm;
        break;
    case OP_SUB:
        // Oduzimanje registara
        // Register subtraction
        cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
        break;
    case OP_BEQ:
        // Grananje ako su registri jednaki
        // Branch if registers are equal
    {
        uint8_t b_rs1 = (instr >> 21) & 0x1F;
        uint8_t b_rs2 = (instr >> 16) & 0x1F;
        int16_t offset = (int16_t)(instr & 0xFFFF);

        printf("[EXEC] BEQ R%d, R%d, offset %d\n", b_rs1, b_rs2, offset);

        if (cpu->regs[b_rs1] == cpu->regs[b_rs2]) {
            // Skok: PC = PC_stari + offset
            // Jump: PC = PC_old + offset
            cpu->pc = (cpu->pc - 4) + offset;
        }
    }
    break;
    case OP_JMP:
    {
        // Bezuslovni skok
        // Unconditional jump
        int16_t offset = (int16_t)(instr & 0xFFFF);
        printf("[EXEC] JMP offset %d\n", offset);
        cpu->pc = (cpu->pc - 4) + offset;
    }
    break;
    default:
        printf("[FAULT] Illegal Opcode: 0x%X\n", opcode);
        cpu->running = false;
        break;
    }
    cpu->regs[0] = 0;
}

void cpu_dump_regs(DREL_CPU* cpu) {
    int i; // Deklaracija van petlje za stari standard | Declaration outside loop for old standard
    printf("\n--- DREL STATE ---\n");
    for (i = 0; i < 8; i++) { 
        printf("R%d: %lld\n", i, cpu->regs[i]);
    }
    printf("PC: %lld\n------------------\n", cpu->pc);
}

// Oslobadanje memorije procesora
// Freeing processor memory
void cpu_free(DREL_CPU* cpu) {
    if (cpu->memory) {
        free(cpu->memory);
        cpu->memory = NULL; // Sigurnosni reset | Safety reset
    }
}
