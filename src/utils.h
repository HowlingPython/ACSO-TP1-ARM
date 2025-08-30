#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include "shell.h"

/* Memory helpers (little-endian) vía mem_read_32/mem_write_32 */
uint64_t mem_read64(uint64_t addr);
void mem_write64(uint64_t addr, uint64_t val);

uint16_t mem_read16(uint64_t addr);
void mem_write16(uint64_t addr, uint16_t val);

uint8_t mem_read8(uint64_t addr);
void mem_write8(uint64_t addr, uint8_t val);

#endif
