#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

/* Memory helpers (little-endian) vía mem_read_32/mem_write_32 */

uint8_t mem_read8(uint64_t addr);
uint16_t mem_read16(uint64_t addr);
uint64_t mem_read64(uint64_t addr);

void mem_write8(uint64_t addr, uint8_t val);
void mem_write16(uint64_t addr, uint16_t val);
void mem_write64(uint64_t addr, uint64_t val);

/* Lectura/escritura de registros X0..X31 (XZR = 31) */
uint64_t read_register(unsigned idx);

void write_register(unsigned idx, uint64_t val);

/* Setea flags N, Z */
void set_nz(uint64_t res);

/* Setea flags N, Z, C y V */
void set_nzcv(uint64_t a, uint64_t b, uint64_t res);

/* Devuelve puntero a array[4] con flags N, Z, V y C */
int *read_nzvc(void); 

#endif