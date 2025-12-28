#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "drel_cpu.h"
#include "drel_asm.h"

// Simulacija hardverskog kašnjenja dekodera
// Hardware decoder penalty simulation
int simulate_x86_decoder_penalty() {
    // x86 dekoder mora da odredi dužinu instrukcije.
    // x86 decoder must determine instruction length.

    // Neka je prosecna instrukcija laka (1 ciklus), ali svaka treca je teška (3-4 ciklusa).
    // Assume average instruction is easy (1 cycle), but every third is complex (3-4 cycles).
    int r = rand() % 100;
    if (r < 70) return 1; // Brzo | Fast (70%)
    if (r < 90) return 2; // Srednje | Medium (20%)
    return 4;             // Sporo/Kompleksno | Slow/Complex (10%)
}

int main() {
    // Inicijalizacija generatora slucajnih brojeva
    // Initialize random number generator
    srand((unsigned int)time(NULL));

    // 1. Priprema DREL programa (Beskonacna petlja koja se vrti N puta)
    // 1. DREL program preparation (Infinite loop running N times)
    // R1 = 1000000 (Brojac | Counter)
    // LOOP:
    //   SUB R1, R1, 1
    //   BEQ R1, R0, EXIT
    //   JMP LOOP
    const char* source_code[] = {
        "LI   R1, 1000",      // R1 = 1000 (Mali broj za test | Small number for testing)
        "LI   R2, 1",         // R2 = 1 (Dekrement | Decrement)
        "LI   R3, 0",         // R3 = 0 (Granicnik | Limit)

        // POCETAK PETLJE | LOOP START (PC=12)
        "SUB  R1, R1, R2",    // [12] R1 = R1 - R2
        "BEQ  R1, R3, 8",     // [16] Ako je R1 == R3, skoci na EXIT | If R1 == R3, jump to EXIT
        "JMP  -8",            // [20] Nazad na SUB | Back to SUB
        "EXIT"                // [24] Kraj programa | End of program
    };

    int lines = sizeof(source_code) / sizeof(source_code[0]);
    uint32_t bin_buffer[128];

    // Asembliranje i cuvanje binarnog fajla
    // Assembling and saving the binary file
    drel_assemble_program(source_code, lines, bin_buffer);
    drel_save_to_bin("bench.drel", bin_buffer, lines);

    // --- SIMULACIJA | SIMULATION ---
    printf("\n=== ARCHITECTURE BENCHMARK (Simulated Decoder Overhead) ===\n");
    printf("Executing loop logic...\n\n");

    long long cycles_drel = 0;
    long long cycles_x86 = 0;

    // Inicijalizacija i ucitavanje virtuelne mašine
    // VM initialization and loading
    DREL_CPU cpu;
    cpu_init(&cpu);
    cpu_load_bin_file(&cpu, "bench.drel");

    // Direktno podešavanje registra za test sa velikim brojem iteracija
    // Direct register setup for high-iteration testing
    cpu.regs[1] = 100000; // 100.000 iteracija | iterations

    while (cpu.running) {
        // Izvrši jedan korak procesora
        // Execute one CPU step
        cpu_step(&cpu);

        // --- MODELOVANJE PERFORMANSI | PERFORMANCE MODELING ---

        // DREL: Fiksna širina (4 bajta) = Predvidiv Fetch = 1 ciklus penala
        // DREL: Fixed width (4 bytes) = Predictable Fetch = 1 cycle penalty
        cycles_drel += 1;

        // x86 Legacy: Varijabilna širina = Pipeline Bubble / Kašnjenje dekodiranja dužine
        // x86 Legacy: Variable width = Pipeline Bubble / Length Decoding penalty
        cycles_x86 += simulate_x86_decoder_penalty();
    }

    // Prikaz rezultata
    // Display results
    printf("\n------------------------------------------------------------\n");
    printf("RESULTS (Lower is Better):\n");
    printf("------------------------------------------------------------\n");
    printf("DREL Decoder Cycles: %lld (Base Line)\n", cycles_drel);
    printf("x86  Decoder Cycles: %lld (Legacy Tax)\n", cycles_x86);
    printf("------------------------------------------------------------\n");

    // Izracunavanje dobitka u efikasnosti
    // Efficiency gain calculation
    double improvement = ((double)(cycles_x86 - cycles_drel) / cycles_x86) * 100.0;
    printf("EFFICIENCY GAIN: +%.2f%% less front-end work\n", improvement);
    printf("------------------------------------------------------------\n");

    // Oslobadanje resursa
    // Resource cleanup
    cpu_free(&cpu);
    return 0;
}
