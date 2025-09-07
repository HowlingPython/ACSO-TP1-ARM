#include "utils.h"

uint8_t mem_read8(uint64_t address) {
    const uint64_t aligned_addr = address & ~UINT64_C(3);
    const uint32_t word32 = mem_read_32(aligned_addr);
    const unsigned byte_index = (unsigned)(address & UINT64_C(3));
    const unsigned bit_shift = byte_index * 8u;
    return (uint8_t)((word32 >> bit_shift) & UINT32_C(0xFF));
}
void mem_write8(uint64_t address, uint8_t value8) {
    const uint64_t aligned_addr = address & ~UINT64_C(3);
    const uint32_t word32 = mem_read_32(aligned_addr);
    const unsigned byte_index = (unsigned)(address & UINT64_C(3));
    const unsigned bit_shift = byte_index * 8u;
    const uint32_t clear_mask = ~(UINT32_C(0xFF) << bit_shift);
    const uint32_t updated_word32 = (word32 & clear_mask) | ((uint32_t)value8 << bit_shift);
    mem_write_32(aligned_addr, updated_word32);
}

uint16_t mem_read16(uint64_t address) {
    const uint64_t aligned_addr = address & ~UINT64_C(3);
    const uint32_t word32 = mem_read_32(aligned_addr);
    const unsigned byte_index = (unsigned)(address & UINT64_C(3));
    const unsigned bit_shift = byte_index * 8u;
    return (uint16_t)((word32 >> bit_shift) & UINT32_C(0xFFFF));
}

void mem_write16(uint64_t address, uint16_t value16) {
    const uint64_t aligned_addr = address & ~UINT64_C(3);
    const uint32_t word32 = mem_read_32(aligned_addr);
    const unsigned byte_index = (unsigned)(address & UINT64_C(3));
    const unsigned bit_shift = byte_index * 8u;
    const uint32_t clear_mask = ~(UINT32_C(0xFFFF) << bit_shift);
    const uint32_t updated_word32 = (word32 & clear_mask) | ((uint32_t)value16 << bit_shift);
    mem_write_32(aligned_addr, updated_word32);
}

uint64_t mem_read64(uint64_t address) {
    uint64_t value64 = 0;
    for (unsigned byte_index = 0; byte_index < 8; ++byte_index) {
        value64 |= ((uint64_t)mem_read8(address + (uint64_t)byte_index)) << (8u * byte_index);
    }
    return value64;
}

void mem_write64(uint64_t address, uint64_t value64) {
    for (unsigned byte_index = 0; byte_index < 8; ++byte_index) {
        const uint8_t byte = (uint8_t)((value64 >> (8u * byte_index)) & UINT64_C(0xFF));
        mem_write8(address + (uint64_t)byte_index, byte);
    }
}
uint64_t read_register(unsigned idx) {
    if (idx == 31)
        return 0; // XZR: lectura siempre 0
    return (uint64_t)CURRENT_STATE.REGS[idx];
}

void write_register(unsigned idx, uint64_t val) {
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