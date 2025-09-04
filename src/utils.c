#include "utils.h"

uint8_t mem_read8(uint64_t addr) {
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    return (uint8_t)((w >> shift) & 0xFFu);
}

void mem_write8(uint64_t addr, uint8_t val) {
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    uint32_t mask = ~(0xFFu << shift);
    uint32_t nw = (w & mask) | (((uint32_t)val) << shift);
    mem_write_32(aligned, nw);
}

uint16_t mem_read16(uint64_t addr) {
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    
    return (uint16_t)((w >> shift) & 0xFFFF);
}

void mem_write16(uint64_t addr, uint16_t val) {
    uint64_t aligned = addr & ~3ULL;
    uint32_t w = mem_read_32(aligned);
    int shift = (int)((addr & 3ULL) * 8);
    uint32_t mask = ~(0xFFFFu << shift);
    uint32_t nw = (w & mask) | (((uint32_t)val) << shift);
    mem_write_32(aligned, nw);
}

uint64_t mem_read64(uint64_t addr) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v |= ((uint64_t)mem_read8(addr + (uint64_t)i)) << (8 * i);
    }
    return v;
}

void mem_write64(uint64_t addr, uint64_t val) {
    for (int i = 0; i < 8; ++i) {
        mem_write8(addr + (uint64_t)i, (uint8_t)((val >> (8 * i)) & 0xFFu));
    }
}

uint64_t read_x(unsigned idx) {
    if (idx == 31)
        return 0; // XZR: lectura siempre 0
    return (uint64_t)CURRENT_STATE.REGS[idx];
}

void write_x(unsigned idx, uint64_t val) {
    if (idx == 31) return;               // XZR: descarta escritura
    NEXT_STATE.REGS[idx] = (int64_t)val; // registros son int64_t en shell.h
}

void set_nz(uint64_t res) {
    NEXT_STATE.FLAG_N = ((int64_t)res < 0);
    NEXT_STATE.FLAG_Z = (res == 0);
}

void set_nzcv(uint64_t a, uint64_t b, uint64_t res) {
    set_nz(res);

    // __uint128_t wide = (__uint128_t)a + (__uint128_t)b;    
    bool sign_a = (a >> 63) & 1;
    bool sign_b = (b >> 63) & 1;
    bool sign_res = (res >> 63) & 1;
    
    // NEXT_STATE.FLAG_C = (wide >> 64) & 1;
    NEXT_STATE.FLAG_V = (sign_a == sign_b) && (sign_a != sign_res);  
    uint64_t result = a + b;  
    NEXT_STATE.FLAG_C = (result < a) || (result < b);
}

int* read_nzvc(void) {
    static int flags[4]; // 0 = N, 1 = Z, 2 = V y 3 = C  
    flags[0] = CURRENT_STATE.FLAG_N;
    flags[1] = CURRENT_STATE.FLAG_Z;
    flags[2] = CURRENT_STATE.FLAG_V;
    flags[3] = CURRENT_STATE.FLAG_C;

    return flags;
}