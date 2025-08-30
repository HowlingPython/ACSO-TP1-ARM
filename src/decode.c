#include "decode.h"

Instruction decode(uint32_t instruction_hex){
    Instruction instruction;
    instruction.hex = instruction_hex;
    
    if (instruction_hex >> 21  == 0x6A2u){
        instruction.opc = HLT;
        return instruction;
    }
    
    instruction.opc = get_opcode(instruction_hex); // enum Opcodes 
}

 Opcode get_opcode(uint32_t instruction) {
    // Primero probar por bits [31:24]
    switch ((instruction >> 24) & 0xFF) {
        // ADDS
        case 0xB1: return ADDS_immediate;
        // SUBS o CMP
        case 0xF1: return (instruction & 0xF) == 0x1F ? CMP_immediate : SUBS_immediate;
        // 
        
        default: break;
    }

    // Luego probamos por bits [31:21]
    switch ((instruction >> 21) & 0xFFF) {
        // ADDS
        case 0x559: return ADDS_extended;
        // SUBS o CMP
        case 0x759: return (instruction & 0xF) == 0x1F ? CMP_extended : SUBS_extended;
        // ANDS shifted
        case 0x750: return ANDS;
        // EOR shifted
        case 0x650: return EOR;
        // OOR shifted
        case 0x550 : return ORR;
        // LSL o LRS
        case : ;
        default: break;
    }

    return UNKNOWN;
}
