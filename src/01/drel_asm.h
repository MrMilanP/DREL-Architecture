#pragma once
#ifndef DREL_ASM_H
#define DREL_ASM_H

#include <stdint.h>

uint32_t drel_assemble_line(const char* line);
int drel_assemble_program(const char** source_lines, int line_count, uint32_t* output_buffer);

// Nova funkcija: Snima buffer kao binarni fajl
int drel_save_to_bin(const char* filename, uint32_t* buffer, int count);

#endif
