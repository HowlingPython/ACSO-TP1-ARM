#include <stdio.h>
#include <inttypes.h>
#include "Opcodes.h"

typedef struct {
    Opcode opc;
    uint32_t hex;
    int rd;
    int rn;
    int rm;
    int imm;
    int shift_amt;
} Instruction;

Instruction decode(uint32_t instruction_hex);
Opcode get_opcode(uint32_t instruction_hex);
