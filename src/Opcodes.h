#ifndef OPCODES_H
#define OPCODES_H

typedef enum
{
    ADDS_extended,
    ADDS_immediate,
    SUBS_extended,
    SUBS_immediate,
    ADD_extended,
    ADD_immediate,
    CMP_extended,
    CMP_immediate,
    ANDS,
    EOR,
    ORR,
    B,
    BR,
    BEQ,
    BNE,
    BGT,
    BLT,
    BGE,
    BLE,
    LSL_immediate,
    LSR_immediate,
    STUR,
    STURB,
    STURH,
    LDUR,
    LDURB,
    LDURH,
    MOVZ,
    MUL,
    CBZ,
    CBNZ,
    HLT,
    UNKNOWN
} Opcode;

#endif
