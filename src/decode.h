#ifndef DECODE_H
#define DECODE_H

#include "shell.h"
#include "Opcodes.h"

typedef struct {
    Opcode opc;
    uint32_t hex;
    uint8_t rd;
    uint8_t rn;
    uint8_t rm;
    int imm;
    int shift_amt;
} Instruction;

Instruction decode(uint32_t instruction_hex);

#endif
