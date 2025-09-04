#include "shell.h"
#include "decode.h"
#include "execute.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory.
     */
    NEXT_STATE = CURRENT_STATE;
    uint32_t instruction_hex = mem_read_32(CURRENT_STATE.PC);
    Instruction instruction = decode(instruction_hex);
    execute(&instruction);
    if (NEXT_STATE.PC == CURRENT_STATE.PC) {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
