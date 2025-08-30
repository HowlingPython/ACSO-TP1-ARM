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

