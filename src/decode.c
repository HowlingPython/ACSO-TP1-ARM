#include "decode.h"

Instruction decode(uint32_t instruction_hex) {
    Instruction instruction = (Instruction){0}; // Inicializar a cero evita basura en campos no seteados
    instruction.hex = instruction_hex;    
    
    // --- HLT: el TP indica [31:21] == 0x6A2.
    if (instruction_hex >> 21  == 0x6A2){
        instruction.opc = HLT;
        return instruction;
    }    

    instruction.opc = get_opcode(instruction_hex); // En "Opcodes.h" esta la enum con los opcodes
    instruction.rd = instruction_hex & 0x1F; // Bits [4:0] Registro destino
    instruction.rn = (instruction_hex >> 5) & 0x1F; // Bits [9:5] Registro fuente 1
    instruction.rm = (instruction_hex >> 16) & 0x1F; // Bits [20:16] Registro fuente 2, solo con Extended y Register
    instruction.imm = get_imm(instruction.opc, instruction_hex); // Inmediato (si aplica)
    instruction.shift_amt = get_shiftamt(instruction.opc, instruction); // Shift amount (si aplica)


    return instruction;
}

Opcode get_opcode(uint32_t instruction) {
    // Slices utiles para clasificar
    uint8_t  cond =  instruction        & 0xF;    // [3:0]  condicion de B.cond
    uint8_t  op24 = (instruction >> 24) & 0xFF;   // [31:24] familias: ADDI/SUBSI/ADDSI/B.cond/CBZ/CBNZ
    uint16_t op21 = (instruction >> 21) & 0x7FF;  // [31:21] familias: R-format, LSU unscaled, logicas

    // --- B (branch incondicional): [31:26] == 0b000101 (0x5)
    if (((instruction >> 26) & 0x3F) == 0x5) return B;
    
    // --- BR (branch register, salto indirecto):
    // Patron fijo con Rn libre en bits [9:5]
    if (((instruction >> 10) & 0x3FFFFF) == 0x3587C0 && (instruction & 0x1F) == 0) return BR;

    // --- LSL/LSR (alias de UBFM 64-bit): [31:22] == 0x34D  (sf=1 y N=1 implicitos).
    // Distincion por campos:
    //   LSR: imms == 63
    //   LSL: imms + 1 == immr      (forma preferida del alias)
    // Nota: si no matchea, es un UBFM generico (fuera del alcance del TP).
    if (((instruction >> 22) & 0x3FF) == 0x34D) {
        uint8_t immr = (instruction >> 16) & 0x3F;
        uint8_t imms = (instruction >> 10) & 0x3F;

        if (imms == 0x3F) return LSR_immediate;
        if ((uint8_t)(imms + 1) == immr) return LSL_immediate;
    }

    // --- MOVZ (move wide w/ zero, 64-bit solo con hw==0):
    // [31:23] == 110100101 (0x1A5) y [22:21] (hw) == 0 ⇒ shift de 0 bits.
    if (((instruction >> 23) & 0x1FF) == 0x1A5) {
        if (((instruction >> 21) & 0x3) == 0) return MOVZ;
    }

    // --- Inmediatas / condicionales por [31:24]
    switch (op24) {
        // ADD (immediate, 64-bit)
        case 0x91: return ADD_immediate;
        // ADDS (immediate, 64-bit). 
        case 0xB1: return ADDS_immediate;
        // SUBS (immediate) o CMP (immediate). CMP es SUBS con Rd==XZR (Rd==31).
        case 0xF1: return ((instruction & 0x1F) == 0x1F) ? CMP_immediate : SUBS_immediate;
        // B.cond (branch condicional). La condicion esta en [3:0] (cond).
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
        // CBZ/CBNZ (64-bit).
        case 0xB4: return CBZ;
        case 0xB5: return CBNZ;
    }

    // --- Registro / Logicas / Memoria por [31:21]
    switch (op21) {
        // ADD/ADDS (extended register).
        case 0x459: return ADD_extended;
        case 0x558: return ADDS_extended; // 0x558 porque el bit 21 es 0 para este TP, sino seria 0x559. 
        // SUBS/CMP (extended register). CMP si Rd==XZR (Rd==31).
        case 0x758: return ((instruction & 0x1F) == 0x1F) ? CMP_extended : SUBS_extended; // igual que en en ADDS.
        // ANDS/EOR/ORR (shifted register). El TP indica ignorar el shift.
        case 0x750: return ANDS;
        case 0x650: return EOR;
        case 0x550: return ORR;
        // MUL
        case 0x4D8: return MUL;
        // Load/Store Unit
        case 0x7C0: return STUR;  // store 64
        case 0x1C0: return STURB; // store byte
        case 0x3C0: return STURH; // store halfword 
        case 0x7C2: return LDUR;  // load  64
        case 0x1C2: return LDURB; // load  byte (zero-extend a 64)
        case 0x3C2: return LDURH; // load  halfword (zero-extend a 64)

    }
    // --- Si no matchea ningun patron conocido, reportar UNKNOWN
    return UNKNOWN;
}

int get_shiftamt(Opcode opc, Instruction instruction) {
    switch (opc) {
        case ADDS_extended:
        case SUBS_extended:
        case CMP_extended:
            return instruction.imm; // shift [12:10], 3 bits  
        case ADDS_immediate:
        case SUBS_immediate: // casos en execute
            return (instruction.hex >> 22) & 0x3; // shift [23:22], 2 bits
        case LSL_immediate:
        case LSR_immediate:
            return (instruction.hex >> 16) & 0x3F; // immr [21:16], 6 bits
        default:
            return 0; // Otros casos no usan shift
    }
}

// Chequear 
int get_imm(Opcode opc, uint32_t instruction) {
    switch (opc) {
        case ADDS_immediate:
        case SUBS_immediate:
        case ADD_immediate:
        case CMP_immediate:
            return (instruction >> 10) & 0xFFF; // imm12 [21:10]
        case B:
            {   // imm26 [25:0], sign-extend y <<2
                int32_t imm26 = instruction & 0x3FFFFFF;
                if (imm26 & 0x2000000) imm26 |= 0xFC000000; // sign-extend
                return imm26 << 2;
            }
        case CBZ:
        case CBNZ:
            {   // imm19 [23:5], sign-extend y <<2
                int32_t imm19 = (instruction >> 5) & 0x7FFFF;
                if (imm19 & 0x40000) imm19 |= 0xFFF80000; // sign-extend 
                return imm19 << 2;
            }
        case STUR:
        case STURB:
        case STURH:
        case LDUR:
        case LDURB:
        case LDURH:
            {   // imm9 [20:12], sign-extend
                int32_t imm9 = (instruction >> 12) & 0x1FF;
                if (imm9 & 0x100) imm9 |= 0xFFFFFE00; // sign-extend
                return imm9;
            }
        case MOVZ:
            {   // imm16 [20:5]
                return (instruction >> 5) & 0xFFFF;
            }
        case BEQ:
        case BNE:
        case BGT:
        case BLT:
        case BGE:
        case BLE:
            {   // imm19 [23:5], sign-extend y <<2
                int32_t imm19 = (instruction >> 5) & 0x7FFFF;
                if (imm19 & 0x40000) imm19 |= 0xFFF80000; // sign-extend 
                return imm19 << 2;
            }
        case ADD_extended:
        case ADDS_extended:
        case CMP_extended:
            {
                return (instruction >> 10) & 0x7; // shift [12:10], 3 bits  
            }
        case SUBS_extended:
        case ANDS:
        case EOR:
        case ORR:
            {
                return (instruction >> 10) & 0x3F; // shift [15:10], 6 bits
            }
        case LSL_immediate:
        case LSR_immediate:
            {
                return (instruction >> 16) & 0x3F; // immr [21:16], 6 bits
            }
        default:
            return 0; // Otros casos no usan inmediato
    }
}