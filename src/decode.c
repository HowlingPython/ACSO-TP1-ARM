#include "decode.h"

Instruction decode(uint32_t instruction_hex) {
    Instruction instruction;
    instruction.hex = instruction_hex;    
    
    if (instruction_hex >> 21  == 0x6A2u){
        instruction.opc = HLT;
        return instruction;
    }    

    instruction.opc = get_opcode(instruction_hex); // Opcodes guradados en un enum


    return instruction;
}

Opcode get_opcode(uint32_t instruction) {
    uint8_t cond = instruction & 0xF;
    uint8_t op24 = (instruction >> 24) & 0xFF;
    uint16_t op21 = (instruction >> 21) & 0x7FF;

    // --- B (unconditional): [31:26] == 0b000101 (0x5)
    if (((instruction >> 26) & 0x3F) == 0x5) return B;
    
    // --- BR (branch register): patrón exacto con Rn libre en [9:5]
    if (((instruction >> 10) & 0x3FFFFF) == 0x3587C0 && (instruction & 0x1F) == 0) {
        return BR;
    }

    // --- LSL/LSR (alias UBFM 64b): [31:22] == 0x34D, distinguir por (immr, imms)
    if (((instruction >> 22) & 0x3FF) == 0x34D) {
        uint8_t immr = (instruction >> 16) & 0x3F;
        uint8_t imms = (instruction >> 10) & 0x3F;

        if (imms == 0x3F) return LSR_immediate;
        if ((uint8_t)(imms + 1) == immr) return LSL_immediate;
    }

    // --- MOVZ 64b: [31:23] == 0x1A5 y hw==0
    if (((instruction >> 23) & 0x1FF) == 0x1A5) {
        if (((instruction >> 21) & 0x3) == 0) return MOVZ;
    }

    // --- Inmediatas / condicionales por [31:24]
    switch (op24) {
        case 0x91: return ADD_immediate;
        
        case 0xB1: return ADDS_immediate;
        case 0xF1: return ((instruction & 0x1F) == 0x1F) ? CMP_immediate : SUBS_immediate;
        
        case 0x54: { // B.cond
            switch (cond) {
                case 0x0: return BEQ;
                case 0x1: return BNE;
                case 0xA: return BGE;
                case 0xB: return BLT;
                case 0xC: return BGT;
                case 0xD: return BLE;
                default: return UNKNOWN;
            }
        }
        
        case 0xB4: return CBZ;
        case 0xB5: return CBNZ;
    }

    // --- Registro / Lógicas / Memoria por [31:21]
    switch (op21) {
        case 0x459: return ADD_extended;
        
        case 0x559: return ADDS_extended;
        case 0x759: return ((instruction & 0x1F) == 0x1F) ? CMP_extended : SUBS_extended;
        
        case 0x750: return ANDS;
        case 0x650: return EOR;
        case 0x550: return ORR;
        
        case 0x1B7: return MUL;

        case 0x7C0: return STUR;
        case 0x1C0: return STURB;
        case 0x3C0: return STURH;

        case 0x7C2: return LDUR;
        case 0x1C2: return LDURB;
        case 0x3C2: return LDURH;

    }

    return UNKNOWN;
}
