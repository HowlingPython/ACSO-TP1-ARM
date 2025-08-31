#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include "shell.h"
#include <stdint.h>

/* Memory helpers (little-endian) vía mem_read_32/mem_write_32 */
uint64_t mem_read64(uint64_t addr);
void mem_write64(uint64_t addr, uint64_t val);

uint16_t mem_read16(uint64_t addr);
void mem_write16(uint64_t addr, uint16_t val);

#endif

#pragma once

/* Lectura/escritura de registros X0..X31 (XZR = 31) */
uint64_t read_x(unsigned idx);
void     write_x(unsigned idx, uint64_t val);

/* Setea N y Z */
void     set_nz(uint64_t res);

int*     read_nz(void);  // devuelve puntero a array[2] con N y Z