#include "utils.h"

uint64_t mem_read64(uint64_t addr)
{
    uint32_t lo = mem_read_32(addr);
    uint32_t hi = mem_read_32(addr + 4);
    return ((uint64_t)hi << 32) | lo;
}
void mem_write64(uint64_t addr, uint64_t val)
{
    uint32_t lo = (uint32_t)(val & 0xFFFFFFFFu);
    uint32_t hi = (uint32_t)(val >> 32);
    mem_write_32(addr, lo);
    mem_write_32(addr + 4, hi);
}

uint16_t mem_read16(uint64_t addr)
{
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    return (uint16_t)((w >> shift) & 0xFFFF);
}
void mem_write16(uint64_t addr, uint16_t val)
{
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    uint32_t mask = ~(0xFFFFu << shift);
    uint32_t nw = (w & mask) | (((uint32_t)val) << shift);
    mem_write_32(aligned, nw);
}

#include <stdint.h>
#include "shell.h"
#include "utils.h"
#include <stdbool.h>

uint64_t read_x(unsigned idx) {
    if (idx == 31) return 0;               // XZR: lectura siempre 0
    return (uint64_t)CURRENT_STATE.REGS[idx];
}

void write_x(unsigned idx, uint64_t val) {
    if (idx == 31) return;                 // XZR: descarta escritura
    NEXT_STATE.REGS[idx] = (int64_t)val;   // registros son int64_t en shell.h
}

void set_nz(uint64_t res) {
    NEXT_STATE.FLAG_N = ((int64_t)res < 0);
    NEXT_STATE.FLAG_Z = (res == 0);
    /* Parte 1: C y V no se usan; si existen en la struct, podés dejarlas en 0 acá: 
       NEXT_STATE.FLAG_C = 0; NEXT_STATE.FLAG_V = 0; */
}

int* read_nz(void) {
    static int flags[2];   // 0 = N, 1 = Z
    flags[0] = CURRENT_STATE.FLAG_N;
    flags[1] = CURRENT_STATE.FLAG_Z;
    return flags;
}