#include "drel_asm.h"
#include "drel_isa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Pomocna funkcija: Pretvara "R3" ili "r3" u broj 3
// Helper function: Converts "R3" or "r3" into number 3
int parse_reg(char* str) {
    if (!str) return 0;
    while (*str && !isdigit(*str)) str++;
    return atoi(str);
}

// Pomocna funkcija: Pretvara tekstualne vrednosti u brojeve
// Helper function: Converts textual values into numbers
int parse_imm(char* str) {
    if (!str) return 0;
    while (*str && !isdigit(*str) && *str != '-') str++;
    return atoi(str);
}

uint32_t drel_assemble_line(const char* line_in) {
    char line[128];
    int i; // Deklaracija na vrhu | Declaration at top
    strcpy(line, line_in);

    // Izdvajanje mnemonika (npr. ADD)
    // Extracting the mnemonic (e.g., ADD)
    char* mnemonic = strtok(line, " ,");
    if (!mnemonic) return 0;

    // Pretvaranje u velika slova
    // Converting to uppercase
    for (i = 0; mnemonic[i]; i++) mnemonic[i] = toupper(mnemonic[i]);

    if (strcmp(mnemonic, "EXIT") == 0) return MAKE_R_TYPE(OP_EXIT, 0, 0, 0);

    if (strcmp(mnemonic, "LI") == 0) {
        // Pseudo instrukcija: LI Rd, Imm
        // Pseudo instruction: LI Rd, Imm
        int rd = parse_reg(strtok(NULL, ","));
        int imm = parse_imm(strtok(NULL, ","));
        return MAKE_I_TYPE(OP_ADDI, rd, 0, imm);
    }

    if (strcmp(mnemonic, "SUB") == 0) {
        // Oduzimanje: SUB Rd, Rs1, Rs2
        // Subtraction: SUB Rd, Rs1, Rs2
        int rd = parse_reg(strtok(NULL, ","));
        int rs1 = parse_reg(strtok(NULL, ","));
        int rs2 = parse_reg(strtok(NULL, ","));
        return MAKE_R_TYPE(OP_SUB, rd, rs1, rs2);
    }

    if (strcmp(mnemonic, "BEQ") == 0) {
        // Grananje: BEQ Rs1, Rs2, Offset
        // Branching: BEQ Rs1, Rs2, Offset
        int rs1 = parse_reg(strtok(NULL, ","));
        int rs2 = parse_reg(strtok(NULL, ","));
        int off = parse_imm(strtok(NULL, ","));
        return MAKE_B_TYPE(OP_BEQ, rs1, rs2, off);
    }

    if (strcmp(mnemonic, "JMP") == 0) {
        // Skok: JMP Offset
        // Jump: JMP Offset
        int off = parse_imm(strtok(NULL, ","));
        return MAKE_B_TYPE(OP_JMP, 0, 0, off);
    }

    return 0; // Greška | Error
}

// Prolazi kroz sve linije koda i sklapa ih u jedan buffer
// Iterates through all code lines and assembles them into a single buffer
int drel_assemble_program(const char** source_lines, int line_count, uint32_t* output_buffer) {
    int i;
    for (i = 0; i < line_count; i++) {
        output_buffer[i] = drel_assemble_line(source_lines[i]);
    }
    return line_count;
}

// Snima asemblirani mašinski kod u binarni .drel fajl
// Saves the assembled machine code into a binary .drel file
int drel_save_to_bin(const char* filename, uint32_t* buffer, int count) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("[ASM Error] Cannot create file: %s\n", filename);
        return 0;
    }
    fwrite(buffer, sizeof(uint32_t), count, f);
    fclose(f);
    printf("[ASM] Binary saved to '%s' (%zu bytes).\n", filename, (size_t)count * sizeof(uint32_t));
    return 1;
}
