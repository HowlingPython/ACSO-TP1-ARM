#include "execute.h"

static void exec_adds_immediate(const Instruction*);
static void exec_adds_extended(const Instruction*);
static void exec_subs_immediate(const Instruction*, int);
static void exec_subs_extended(const Instruction*, int);
static void exec_hlt(const Instruction*);
static void exec_cmp_immediate(const Instruction*);
static void exec_cmp_extended(const Instruction*);
static void exec_ands(const Instruction*);
static void exec_eor(const Instruction*);
static void exec_orr(const Instruction*);
static void exec_b(const Instruction*);
static void exec_br(const Instruction*);
static void exec_beq(const Instruction*);
static void exec_bne(const Instruction*);
static void exec_bgt(const Instruction*);
static void exec_blt(const Instruction*);
static void exec_bge(const Instruction*);
static void exec_ble(const Instruction*);
static void exec_lsl_immediate(const Instruction*);
static void exec_lsr_immediate(const Instruction*);
static void exec_movz(const Instruction*);
static void exec_add_immediate(const Instruction*);
static void exec_add_extended(const Instruction*);
static void exec_mul(const Instruction*);
static void exec_cbz(const Instruction*);
static void exec_cbnz(const Instruction*);
static void exec_stur(const Instruction*);
static void exec_sturb(const Instruction*);
static void exec_sturh(const Instruction*);
static void exec_ldur(const Instruction*);
static void exec_ldurb(const Instruction*);
static void exec_ldurh(const Instruction*);
static void exec_adcs(const Instruction*); // no implementado
static void handle_unknown(const Instruction*);

static void exec_adds_immediate(const Instruction* in) {
    uint64_t a   = read_x(in->rn);
    uint64_t imm = (uint64_t)(in->imm & 0xFFF);   // imm12

    /* implementar shift == 00 (sin cambio) y 01 (<<12) no ReservedValue */
    if (in->shift_amt == 1) imm <<= 12;

    uint64_t r = a + imm;
    write_x(in->rd, r);
    set_nz(r);
    set_v(a, imm, r);
    set_c(a, imm);
}

static void exec_adds_extended(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);   // ignorar extend/amount (asumir 0)
    uint64_t r = a + b;

    write_x(in->rd, r);
    // set_c(a, b, r);
    set_nz(r);
    set_v(a, b, r);
    set_c(a, b);
}

static void exec_subs_immediate(const Instruction* in, int save_result) {
    uint64_t a   = read_x(in->rn);
    uint64_t imm = (uint64_t)(in->imm & 0xFFF);   // imm12

    /* implementar shift == 00 (sin cambio) y 01 (<<12) no ReservedValue */
    if (in->shift_amt == 1) imm <<= 12;

    uint64_t r = a - imm;
    if (save_result) write_x(in->rd, r);
    set_nz(r);
    set_v(a, imm, r);
    set_c(a, imm);
}

static void exec_subs_extended(const Instruction* in, int save_result) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);   // ignorar extend/amount (asumir 0)
    uint64_t r = a - b;

    if (save_result) write_x(in->rd, r);
    set_nz(r);
    set_v(a, b, r);
    set_c(a, b);
}

static void exec_hlt(const Instruction* in) {
    (void)in;
    RUN_BIT = FALSE;
}

static void exec_cmp_immediate(const Instruction* in) {
    exec_subs_immediate(in, FALSE);  // no guarda resultado
}

static void exec_cmp_extended(const Instruction* in) {
    exec_subs_extended(in, FALSE);  // no guarda resultado  
}

static void exec_ands (const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);
    uint64_t r = a & b;

    write_x(in->rd, r);
    set_nz(r);
}

static void exec_eor (const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);
    uint64_t r = a ^ b;

    write_x(in->rd, r);
}

static void exec_orr (const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);
    uint64_t r = a | b;

    write_x(in->rd, r);
}

static void exec_b (const Instruction* in) {
    NEXT_STATE.PC = CURRENT_STATE.PC + in->imm; // in->imm ya esta <<2 y sign-extended
}

static void exec_br (const Instruction* in) {
    uint64_t target = read_x(in->rn);
    NEXT_STATE.PC = target;
}
static void exec_beq(const Instruction* in) {
    // Salta si Z == 1
    if (CURRENT_STATE.FLAG_Z) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo    
}

static void exec_bne(const Instruction* in) {
    // Salta si Z == 0
    if (!CURRENT_STATE.FLAG_Z) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
}

static void exec_bgt(const Instruction* in) {
    // Salta si Z == 0 y N == V (N y V son 0 en este TP)
    if (!CURRENT_STATE.FLAG_Z && (CURRENT_STATE.FLAG_N == 0)) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
}

static void exec_blt(const Instruction* in) {
    // Salta si N != V (N y V son 0 en este TP)
    if (CURRENT_STATE.FLAG_N != 0) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
}

static void exec_bge(const Instruction* in) {
    // Salta si N == V (N y V son 0 en este TP)
    if (CURRENT_STATE.FLAG_N == 0) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
}

static void exec_ble(const Instruction* in) {
    // Salta si Z == 1 o N != V (N y V son 0 en este TP)
    if (CURRENT_STATE.FLAG_Z || (CURRENT_STATE.FLAG_N != 0)) NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
}

static void exec_lsl_immediate(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t r = a << in->shift_amt;  // shift_amt computado en el decode

    write_x(in->rd, r);
}

static void exec_lsr_immediate(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t r = a >> in->shift_amt;  // shift_amt computado en el decode

    write_x(in->rd, r);
}

static void exec_movz(const Instruction* in) {
    // uint64_t imm = (uint64_t)(in->imm & 0xFFFF); // imm16

    // El TP indica hw==0, asi que no hay que hacer shift,
    // pero si no seria: 
    uint64_t v = ((uint64_t)(in->imm & 0xFFFF)) << in->shift_amt;
    write_x(in->rd, v);

    // write_x(in->rd, imm);
}

static void exec_add_immediate(const Instruction* in) {
    uint64_t a   = read_x(in->rn);
    uint64_t imm = (uint64_t)(in->imm & 0xFFF);   // imm12

    /* implementar shift == 00 (sin cambio) y 01 (<<12) no ReservedValue */
    if (in->shift_amt == 1) imm <<= 12;

    uint64_t r = a + imm;
    write_x(in->rd, r);
}

static void exec_add_extended(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);   // ignorar extend/amount (asumir 0)
    uint64_t r = a + b;

    write_x(in->rd, r);
}

static void exec_mul(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);
    uint64_t r = a * b;

    write_x(in->rd, r);
}

static void exec_cbz(const Instruction* in) {
    uint64_t a = read_x(in->rd); // Rt esta en [4:0]
    if (a == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
    }
}

static void exec_cbnz(const Instruction* in) {
    uint64_t a = read_x(in->rd); // Rt esta en [4:0]
    if (a != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + in->imm;   // in->imm ya <<2 y con signo
    }
}

static void exec_stur(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm; // imm9 sign-extended
    uint64_t v = read_x(in->rd);
    mem_write64(addr, v);
}

static void exec_sturb(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm;
    uint8_t v = (uint8_t)read_x(in->rd);
    mem_write8(addr, v);
}

static void exec_sturh(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm;
    uint16_t v = (uint16_t)read_x(in->rd);
    mem_write16(addr, v);
}

static void exec_ldur(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm;
    uint64_t v = mem_read64(addr);
    write_x(in->rd, v);
}

static void exec_ldurb(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm;
    uint64_t v = (uint64_t)mem_read8(addr); // convertir de un entero unsigned mas chico a 
    write_x(in->rd, v);                     // uno unsigned mas grande siempre hace zero-extend
}

static void exec_ldurh(const Instruction* in) {
    uint64_t addr = read_x(in->rn) + (int64_t)in->imm;
    uint64_t v = (uint64_t)mem_read16(addr); // zero-extend
    write_x(in->rd, v);
}

static void exec_adcs(const Instruction* in) {
    uint64_t a = read_x(in->rn);
    uint64_t b = read_x(in->rm);   // ignorar extend/amount (asumir 0)
    uint64_t c = (uint64_t)CURRENT_STATE.FLAG_C; // llevar como 0 o 1
    uint64_t r = a + b + c;

    write_x(in->rd, r);
    //  set_nz(r); //preguntar si hay q ponerlo o no
    // set_c(a, b, r); // No se pide actualizar C
}

// Falla en opcodes no reconocidos
static void handle_unknown(const Instruction* in) {
    fprintf(stderr,
            "[SIM][EXCEPTION] Unknown/unsupported opcode\n"
            "  PC=0x%016llx  HEX=0x%08x  OPC=%d\n",
            (unsigned long long)CURRENT_STATE.PC,
            (unsigned)in->hex,
            (int)in->opc);
    fflush(stderr);
    exit(EXIT_FAILURE);    
}

void execute(const Instruction* in) {
    switch (in->opc) {
        case HLT:            exec_hlt(in);            break;
        // case UNKNOWN:        handle_unknown(in);      break;
        case ADCS:          /* no implementado */        break;
        case ADDS_immediate: exec_adds_immediate(in); break;
        case ADDS_extended:  exec_adds_extended(in);  break;
        case SUBS_immediate: exec_subs_immediate(in, TRUE); break;
        case SUBS_extended:  exec_subs_extended(in, TRUE);  break;
        case CMP_immediate:  exec_cmp_immediate(in);  break;
        case CMP_extended:   exec_cmp_extended(in);   break;
        case ANDS:           exec_ands(in);           break;
        case EOR:            exec_eor(in);            break;
        case ORR:            exec_orr(in);            break;
        case B:              exec_b(in);              break;
        case BR:             exec_br(in);             break;
        case BEQ:            exec_beq(in);            break;
        case BNE:            exec_bne(in);            break;
        case BGT:            exec_bgt(in);            break;
        case BLT:            exec_blt(in);            break;
        case BGE:            exec_bge(in);            break;
        case BLE:            exec_ble(in);            break;
        case LSL_immediate:  exec_lsl_immediate(in);  break;
        case LSR_immediate:  exec_lsr_immediate(in);  break;
        case MOVZ:           exec_movz(in);           break;
        case ADD_immediate:  exec_add_immediate(in);  break;
        case ADD_extended:   exec_add_extended(in);   break;
        case MUL:            exec_mul(in);            break;
        case CBZ:            exec_cbz(in);            break;
        case CBNZ:           exec_cbnz(in);           break;
        case STUR:           exec_stur(in);           break;
        case STURB:          exec_sturb(in);          break;
        case STURH:          exec_sturh(in);          break;
        case LDUR:           exec_ldur(in);           break;
        case LDURB:          exec_ldurb(in);          break;
        case LDURH:          exec_ldurh(in);          break;
        default: 0;
    }
}
