#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "drel_cpu.h"
#include "drel_asm.h"

// Simulacija hardverskog kašnjenja dekodera
// Hardware decoder penalty simulation
int simulate_x86_decoder_penalty() {
    int r = rand() % 100;
    if (r < 70) return 1; // Brzo | Fast (70%)
    if (r < 90) return 2; // Srednje | Medium (20%)
    return 4;             // Sporo/Kompleksno | Slow/Complex (10%)
}

int main() {
    // Inicijalizacija generatora slucajnih brojeva
    // Initialize random number generator
    srand((unsigned int)time(NULL));

    // DREL program (inicijalizacija + petlja)
    // DREL program (init + loop)
    // Napomena: inicijalizaciju (prva 3 LI) NECEMO izvrsavati u benchmark-u,
    // Note: we will NOT execute the first 3 LI init instructions in benchmark,
    // jer hocemo veliki brojac da setujemo direktno u registru.
    // because we want to set a large counter directly in the register.
    const char* source_code[] = {
        "LI   R1, 1000",      // (ne koristi se u benchmark-u)
                              // (not used in benchmark)
        "LI   R2, 1",         // (ne koristi se u benchmark-u)
                              // (not used in benchmark)
        "LI   R3, 0",         // (ne koristi se u benchmark-u)
                              // (not used in benchmark)
        
        // POCETAK PETLJE | LOOP START (PC = 12 = 3 instr * 4 bajta)
        // LOOP START (PC = 12 = 3 instr * 4 bytes)
        "SUB  R1, R1, R2",    // [12] R1 = R1 - 1
                              // [12] R1 = R1 - 1
        "BEQ  R1, R3, 8",     // [16] Ako je R1 == 0, skoci na EXIT
                              // [16] If R1 == 0, jump to EXIT
        "JMP  -8",            // [20] Nazad na SUB
                              // [20] Back to SUB
        "EXIT"                // [24]
                              // [24]
    };

    int lines = (int)(sizeof(source_code) / sizeof(source_code[0]));
    uint32_t bin_buffer[128];

    // Asembliranje i cuvanje binarnog fajla
    // Assembling and saving the binary file
    drel_assemble_program(source_code, lines, bin_buffer);
    drel_save_to_bin("bench.drel", bin_buffer, lines);

    printf("\n=== ARCHITECTURE BENCHMARK (Simulated Decoder Overhead) ===\n");
    printf("Executing loop logic...\n\n");

    long long cycles_drel = 0;
    long long cycles_x86 = 0;

    // VM init i load
    // VM init and load
    DREL_CPU cpu;
    cpu_init(&cpu);
    cpu_load_bin_file(&cpu, "bench.drel");

    // ------------------------------------------------------------
    // KLJUCNA IZMENA: preskoci inicijalne LI instrukcije
    // KEY CHANGE: skip initial LI instructions
    // i rucno namesti registre + PC na pocetak petlje.
    // and manually set registers + PC to the start of the loop.
    // ------------------------------------------------------------
    cpu.regs[1] = 100000;  // BROJ ITERACIJA (menjaj ovde koliko hoces)
                           // ITERATION COUNT (change here as you want)
    cpu.regs[2] = 1;       // decrement
                           // decrement
    cpu.regs[3] = 0;       // limit (0)
                           // limit (0)
    cpu.pc = 12;           // start petlje (posle 3*4 bajta)
                           // loop start (after 3*4 bytes)
    // ------------------------------------------------------------

    while (cpu.running) {
        cpu_step(&cpu);

        // DREL: fiksna sirina -> 1 “cycle” po instrukciji u tvom modelu
        // DREL: fixed width -> 1 “cycle” per instruction in your model
        cycles_drel += 1;

        // x86: simulirani legacy tax
        // x86: simulated legacy tax
        cycles_x86 += simulate_x86_decoder_penalty();
    }

    printf("\n------------------------------------------------------------\n");
    printf("RESULTS (Lower is Better):\n");
    printf("------------------------------------------------------------\n");
    printf("DREL Decoder Cycles: %lld (Base Line)\n", cycles_drel);
    printf("x86  Decoder Cycles: %lld (Legacy Tax)\n", cycles_x86);
    printf("------------------------------------------------------------\n");

    double improvement = ((double)(cycles_x86 - cycles_drel) / (double)cycles_x86) * 100.0;
    printf("EFFICIENCY GAIN: +%.2f%% less front-end work\n", improvement);
    printf("------------------------------------------------------------\n");

    cpu_free(&cpu);
    return 0;
}
