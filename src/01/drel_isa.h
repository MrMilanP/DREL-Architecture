#ifndef DREL_ISA_H
#define DREL_ISA_H

#include <stdint.h>

// Instrukcije
// Instructions
typedef enum {
    OP_EXIT = 0x00, // Zaustavlja izvršavanje | Halts execution
    OP_ADD = 0x01, // Sabiranje registara | Register addition
    OP_ADDI = 0x02, // Sabiranje sa neposrednom vrednošcu | Addition with immediate value
    OP_LI = 0x03, // Ucitavanje neposredne vrednosti | Load immediate value
    OP_BEQ = 0x04, // Grananje ako su jednaki | Branch if equal
    OP_JMP = 0x05, // Bezuslovni skok | Unconditional jump
    OP_SUB = 0x06  // Oduzimanje registara | Register subtraction
} DREL_Opcode;

// Makroi za pakovanje instrukcija
// Instruction packing macros
#define MAKE_R_TYPE(op, rd, rs1, rs2) \
    ((op & 0x3F) << 26 | (rd & 0x1F) << 21 | (rs1 & 0x1F) << 16 | (rs2 & 0x1F) << 11)

#define MAKE_I_TYPE(op, rd, rs1, imm) \
    ((op & 0x3F) << 26 | (rd & 0x1F) << 21 | (rs1 & 0x1F) << 16 | (imm & 0xFFFF))

// Makro za skokove (B-Type)
// Macro for branches (B-Type)
#define MAKE_B_TYPE(op, rs1, rs2, imm) \
    ((op & 0x3F) << 26 | (rs1 & 0x1F) << 21 | (rs2 & 0x1F) << 16 | (imm & 0xFFFF))

#endif
