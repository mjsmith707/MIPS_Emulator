//
//  CPU.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "CPU.h"

// String tables for readable instruction decoding
const char* CPU::opcodeNames[64] {
/* 0x00 */ "invalid", "invalid", "j", "jal", "beq", "bne", "blez", "bgtz", "addi", "addiu",
/* 0x0A */ "slti", "sltiu", "andi", "ori", "xori", "lui",
/* 0x10 */ "invalid", "cop1", "cop2", "invalid", "beql", "bnel", "blezl", "bgtzl", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "special2", "invalid", "invalid", "invalid",
/* 0x20 */ "lb", "lh", "lwl", "lw", "lbu", "lhu", "lwr", "invalid", "sb", "sh",
/* 0x2A */ "swl", "sw", "invalid", "invalid", "swr", "cache",
/* 0x30 */ "ll", "lwc1", "lwc2", "pref", "invalid", "ldc1", "ldc2", "invalid", "sc", "swc1",
/* 0x3A */ "swc2", "invalid", "invalid", "sdc1", "sdc2", "invalid"
};

const char* CPU::functNames[64] {
/* 0x00 */ "sll", "invalid", "srl", "sra", "sllv", "srlv", "srav", "jr", "jalr", "invalid",
/* 0x0A */ "movz", "movn", "syscall", "break", "invalid", "sync",
/* 0x10 */ "mfhi", "mthi", "mflo", "mtlo", "invalid", "invalid", "invalid", "invalid", "mult", "multu",
/* 0x1A */ "div", "divu", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "add", "addu", "sub", "subu", "and", "or", "xor", "nor", "invalid", "invalid",
/* 0x2A */ "slt", "sltu", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "tge", "tgeu", "tlt", "tltu", "teq", "invalid", "tne", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::special2Names[64] {
/* 0x00 */ "madd", "maddu", "mul", "invalid", "msub", "msubu", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "clz", "clo", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x2A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "sdbbp"
};

const char* CPU::special3Names[64] {
/* 0x00 */ "ext", "invalid", "invalid", "invalid", "ins", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "bshfl", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x2A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::regimmNames[32] {
/* 0x00 */ "bltz", "bgez", "bltzl", "bgezl", "invalid", "invalid", "invalid", "invalid", "tgei", "tgeiu",
/* 0x0A */ "tlti", "tltiu", "teqi", "invalid", "tnei", "invalid",
/* 0x10 */ "bltzal", "bgezal", "bltzall", "bgezall", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::cop0Names[32] {
/* 0x00 */ "mfc0", "invalid", "invalid", "invalid", "mtc0", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "di", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "eret", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "deret"
};

const char* CPU::cop0CONames[64] {
/* 0x00 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x2A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::registerNames[32] {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

// Parameterized Constructor
// Attaches the CPU to other devices
CPU::CPU(ConsoleUI* conUI, PMMU* memory) : consoleUI(conUI), memory(memory), cycleCounter(0), signal(0), exceptRestartLoop(false) {
    for (int i=0; i<32; i++) {
        registers[i] = 0;
    }
}

CPU::CPU(PMMU* memory) : consoleUI(nullptr), memory(memory), cycleCounter(0), signal(0), exceptRestartLoop(false) {
    for (int i=0; i<32; i++) {
        registers[i] = 0;
    }
}

// Sets the initial program counter for the CPU
void CPU::setPC(uint32_t addr) {
    PC = addr;
}

// Public starting function for the CPU
void CPU::start() {
    dispatchLoop();
}

// Public function to signal to the thread bound to the cpu
// This is very very temporary until proper interrupts are in
void CPU::sendSignal(uint32_t sig) {
    signal = sig;
}

// Returns the control coprocessor associated with this CPU
// Mainly for loading elf files into memory
Coprocessor0* CPU::getControlCoprocessor() {
    return &cop0_processor;
}

// For unit testing interface
#ifdef TEST_PROJECT
void CPU::stepCPU(uint32_t amount) {
    for (uint32_t i=0; i<amount; i++) {
        start();
    }
}
uint32_t CPU::getPC() {
    return this->PC;
}
uint32_t CPU::getIR() {
    return this->IR;
}
uint32_t CPU::getHI() {
    return this->HI;
}
uint32_t CPU::getLO() {
    return this->LO;
}
uint32_t CPU::getRegister(uint8_t num) {
    return this->registers[num];
}
uint8_t CPU::getOpcode() {
    return this->opcode;
}
uint8_t CPU::getRS() {
    return this->rs;
}
uint8_t CPU::getRT() {
    return this->rt;
}
uint8_t CPU::getRD() {
    return this->rd;
}
uint8_t CPU::getShamt() {
    return this->shamt;
}
uint8_t CPU::getFunct() {
    return this->funct;
}
uint16_t CPU::getImm() {
    return this->imm;
}
int32_t CPU::getImmSE() {
    return this->immse;
}
uint32_t CPU::getJimm() {
    return this->jimm;
}
uint8_t CPU::getSel() {
    return this->sel;
}
void CPU::setIR(uint32_t val) {
    this->IR = val;
}
void CPU::setHI(uint32_t val) {
    this->HI = val;
}
void CPU::setLO(uint32_t val) {
    this->LO = val;
}
void CPU::setRegister(uint8_t num, uint32_t val) {
    this->registers[num] = val;
}
void CPU::setOpcode(uint8_t val) {
    this->opcode = val;
}
void CPU::setRS(uint8_t val) {
    this->rs = val;
}
void CPU::setRT(uint8_t val) {
    this->rt = val;
}
void CPU::setRD(uint8_t val) {
    this->rd = val;
}
void CPU::setShamt(uint8_t val) {
    this->shamt = val;
}
void CPU::setFunct(uint8_t val) {
    this->funct = val;
}
void CPU::setImm(uint16_t val) {
    this->imm = val;
}
void CPU::setImmSE(int32_t val) {
    this->immse = val;
}
void CPU::setJimm(uint32_t val) {
    this->jimm = val;
}
void CPU::setSel(uint8_t val) {
    this->sel = val;
}
#endif

// Prints information about the cpu's execution per cycle
// Also does on the fly disassembly
void CPU::debugPrint() {
    decodeAll();
    std::stringstream ss;
    ss << "=== Decode ===" << std::endl
    << "Cycle = " << std::dec << cycleCounter << std::endl
    << std::hex << "PC = 0x" << PC-4 << std::endl
    << "IR = 0x" << IR << std::endl
    << "opcode = 0x" << (uint16_t)opcode << std::endl << std::dec
    << "rs = " << (uint16_t)rs << std::endl
    << "rt = " << (uint16_t)rt << std::endl
    << "rd = " << (uint16_t)rd << std::endl
    << "shamt = " << (int16_t)shamt << std::endl
    << "funct = " << (uint16_t)funct << std::endl
    << "imm = " << imm << std::endl
    << "immse = " << immse << std::endl
    << "jimm = 0x" << std::hex << (int)jimm << std::endl
    << "sel = " << std::dec << (int)sel << std::endl
    << "=== END Decode ===" << std::endl;
    ss << std::dec << "=== Disassembly ===" << std::endl;
    if (branchDelay) {
        ss << "-- Delay Slot --" << std::endl;
    }
    switch (opcode) {
        case 0x0: {
            ss << "SPECIAL RD RS RT" << std::endl
            << functNames[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        case 0x1: {
            ss << "REGIMM RS IMM" << std::endl
            << regimmNames[rt] << " " << registerNames[rs] << ", " << imm << std::endl;
            break;
        }
        case 0x10: {
            ss << "COP0 RT RD SEL" << std::endl
            << cop0Names[rs] << " " << registerNames[rt] << ", " << registerNames[rd] << ", " << (imm&0x7) << std::endl;
            break;
        }
        case 0x1C: {
            ss << "SPECIAL2 RD RS RT" << std::endl
            << special2Names[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        case 0x1F: {
            ss << "SPECIAL3 RD RS RT" << std::endl
            << special3Names[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        default: {
            ss << "OPCODE RT RS IMM/JIMM" << std::endl
            << opcodeNames[opcode] << " " << registerNames[rt] << ", " << registerNames[rs] << ", " << imm << std::hex << ", 0x" << jimm << std::endl;
            break;
        }
    }
    ss << "=== END Disassembly ===" << std::endl << std::endl;
    if (consoleUI != nullptr) {
        //consoleUI->sendConsoleMsg(ss.str());
        std::cout << ss.str() << std::endl;
    }
}

// Fetches the next instruction into the program counter and instruction register
#define fetch()  \
    cycleCounter++; \
\
    registers[0] = 0; \
\
    if (signal != 0) { \
        goto CPU_HALT; \
    } \
\
    if (branch) { \
        branch = false; \
        branchDelay = false; \
        PC = branchAddr; \
    } \
\
    memory->readWord(PC, &IR, &cop0_processor); \
    PC += 4; \
\
    if (branchDelay) { \
        branch = true; \
    } \


// Decodes all information from the instruction
// Mainly for debugging
__attribute__((always_inline)) void CPU::decodeAll() {
    DECODE_OPCODE();
    DECODE_RS();
    DECODE_RT();
    DECODE_RD();
    DECODE_SHAMT();
    DECODE_FUNCT();
    DECODE_IMM();
    DECODE_IMMSE();
    DECODE_JIMM();
    DECODE_SEL();
}

// CPU Dispatch loop
// Using computed goto for high performance at the expense of readability
void CPU::dispatchLoop() {
    // Computed Goto Tables
    static void* opcodeTable[64] = {
        &&FUNCT,    // 0x00
        &&REGIMM,   // 0x01
        &&J,        // 0x02
        &&JAL,      // 0x03
        &&BEQ,      // 0x04
        &&BNE,      // 0x05
        &&BLEZ,     // 0x06
        &&BGTZ,     // 0x07
        &&ADDI,     // 0x08
        &&ADDIU,    // 0x09
        &&SLTI,     // 0x0A
        &&SLTIU,    // 0x0B
        &&ANDI,     // 0x0C
        &&ORI,      // 0x0D
        &&XORI,     // 0x0E
        &&LUI,      // 0x0F
        &&COP0,     // 0x10
        &&COP1,     // 0x11
        &&COP2,     // 0x12
        &&INVALID_INSTRUCTION,          // 0x13
        &&BEQL,     // 0x14
        &&BNEL,     // 0x15
        &&BLEZL,    // 0x16
        &&BGTZL,    // 0x17
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&SPECIAL2, // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&SPECIAL3, // 0x1F
        &&LB,       // 0x20
        &&LH,       // 0x21
        &&LWL,      // 0x22
        &&LW,       // 0x23
        &&LBU,      // 0x24
        &&LHU,      // 0x25
        &&LWR,      // 0x26
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x27
        &&SB,       // 0x28
        &&SH,       // 0x29
        &&SWL,      // 0x2A
        &&SW,       // 0x2B
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x2C
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x2D
        &&SWR,      // 0x2E
        &&CACHE,    // 0x2F
        &&LL,       // 0x30
        &&LWC1,     // 0x31
        &&LWC2,     // 0x32
        &&PREF,     // 0x33
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x34
        &&LDC1,     // 0x35
        &&LDC2,     // 0x36
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x37
        &&SC,       // 0x38
        &&SWC1,     // 0x39
        &&SWC2,     // 0x3A
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3B
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3C
        &&SDC1,     // 0x3D
        &&SDC2,     // 0x3E
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3F
    };
    
    static void* functTable[64] = {
        &&SLL,      // 0x00
        &&INVALID_INSTRUCTION,          // 0x01
        &&SRL,      // 0x02
        &&SRA,      // 0x03
        &&SLLV,     // 0x04
        &&INVALID_INSTRUCTION,          // 0x05
        &&SRLV,     // 0x06
        &&SRAV,     // 0x07
        &&JR,       // 0x08
        &&JALR,     // 0x09
        &&MOVZ,     // 0x0A
        &&MOVN,     // 0x0B
        &&SYSCALL,  // 0x0C
        &&BREAK,    // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
        &&SYNC,     // 0x0F
        &&MFHI,     // 0x10
        &&MTHI,     // 0x11
        &&MFLO,     // 0x12
        &&MTLO,     // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&MULT,     // 0x18
        &&MULTU,    // 0x19
        &&DIV,      // 0x1A
        &&DIVU,     // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
        &&ADD,      // 0x20
        &&ADDU,     // 0x21
        &&SUB,      // 0x22
        &&SUBU,     // 0x23
        &&AND,      // 0x24
        &&OR,       // 0x25
        &&XOR,      // 0x26
        &&NOR,      // 0x27
        &&INVALID_INSTRUCTION,          // 0x28
        &&INVALID_INSTRUCTION,          // 0x29
        &&SLT,      // 0x2A
        &&SLTU,     // 0x2B
        &&INVALID_INSTRUCTION,          // 0x2C
        &&INVALID_INSTRUCTION,          // 0x2D
        &&INVALID_INSTRUCTION,          // 0x2E
        &&INVALID_INSTRUCTION,          // 0x2F
        &&TGE,      // 0x30
        &&TGEU,     // 0x31
        &&TLT,      // 0x32
        &&TLTU,     // 0x33
        &&TEQ,      // 0x34
        &&INVALID_INSTRUCTION,          // 0x35
        &&TNE,      // 0x36
        &&INVALID_INSTRUCTION,          // 0x37
        &&INVALID_INSTRUCTION,          // 0x38
        &&INVALID_INSTRUCTION,          // 0x39
        &&INVALID_INSTRUCTION,          // 0x3A
        &&INVALID_INSTRUCTION,          // 0x3B
        &&INVALID_INSTRUCTION,          // 0x3C
        &&INVALID_INSTRUCTION,          // 0x3D
        &&INVALID_INSTRUCTION,          // 0x3E
        &&INVALID_INSTRUCTION,          // 0x3F
    };
    
    static void* special2Table[64] {
        &&MADD,     // 0x00
        &&MADDU,    // 0x01
        &&MUL,      // 0x02
        &&INVALID_INSTRUCTION,          // 0x03
        &&MSUB,     // 0x04
        &&MSUBU,    // 0x05
        &&INVALID_INSTRUCTION,          // 0x06
        &&INVALID_INSTRUCTION,          // 0x07
        &&INVALID_INSTRUCTION,          // 0x08
        &&INVALID_INSTRUCTION,          // 0x09
        &&INVALID_INSTRUCTION,          // 0x0A
        &&INVALID_INSTRUCTION,          // 0x0B
        &&INVALID_INSTRUCTION,          // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
        &&INVALID_INSTRUCTION,          // 0x0F
        &&INVALID_INSTRUCTION,          // 0x10
        &&INVALID_INSTRUCTION,          // 0x11
        &&INVALID_INSTRUCTION,          // 0x12
        &&INVALID_INSTRUCTION,          // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
        &&CLZ,      // 0x20
        &&CLO,      // 0x21
        &&INVALID_INSTRUCTION,          // 0x22
        &&INVALID_INSTRUCTION,          // 0x23
        &&INVALID_INSTRUCTION,          // 0x24
        &&INVALID_INSTRUCTION,          // 0x25
        &&INVALID_INSTRUCTION,          // 0x26
        &&INVALID_INSTRUCTION,          // 0x27
        &&INVALID_INSTRUCTION,          // 0x28
        &&INVALID_INSTRUCTION,          // 0x29
        &&INVALID_INSTRUCTION,          // 0x2A
        &&INVALID_INSTRUCTION,          // 0x2B
        &&INVALID_INSTRUCTION,          // 0x2C
        &&INVALID_INSTRUCTION,          // 0x2D
        &&INVALID_INSTRUCTION,          // 0x2E
        &&INVALID_INSTRUCTION,          // 0x2F
        &&INVALID_INSTRUCTION,          // 0x30
        &&INVALID_INSTRUCTION,          // 0x31
        &&INVALID_INSTRUCTION,          // 0x32
        &&INVALID_INSTRUCTION,          // 0x33
        &&INVALID_INSTRUCTION,          // 0x34
        &&INVALID_INSTRUCTION,          // 0x35
        &&INVALID_INSTRUCTION,          // 0x36
        &&INVALID_INSTRUCTION,          // 0x37
        &&INVALID_INSTRUCTION,          // 0x38
        &&INVALID_INSTRUCTION,          // 0x39
        &&INVALID_INSTRUCTION,          // 0x3A
        &&INVALID_INSTRUCTION,          // 0x3B
        &&INVALID_INSTRUCTION,          // 0x3C
        &&INVALID_INSTRUCTION,          // 0x3D
        &&INVALID_INSTRUCTION,          // 0x3E
        &&SDBBP,    // 0x3F
    };
    
    static void* special3Table[64] {
        &&EXT,      // 0x00
        &&INVALID_INSTRUCTION,          // 0x01
        &&INVALID_INSTRUCTION,          // 0x02
        &&INVALID_INSTRUCTION,          // 0x03
        &&INS,      // 0x04
        &&INVALID_INSTRUCTION,          // 0x05
        &&INVALID_INSTRUCTION,          // 0x06
        &&INVALID_INSTRUCTION,          // 0x07
        &&INVALID_INSTRUCTION,          // 0x08
        &&INVALID_INSTRUCTION,          // 0x09
        &&INVALID_INSTRUCTION,          // 0x0A
        &&INVALID_INSTRUCTION,          // 0x0B
        &&INVALID_INSTRUCTION,          // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
        &&INVALID_INSTRUCTION,          // 0x0F
        &&INVALID_INSTRUCTION,          // 0x10
        &&INVALID_INSTRUCTION,          // 0x11
        &&INVALID_INSTRUCTION,          // 0x12
        &&INVALID_INSTRUCTION,          // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
        &&BSHFL,    // 0x20
        &&INVALID_INSTRUCTION,          // 0x21
        &&INVALID_INSTRUCTION,          // 0x22
        &&INVALID_INSTRUCTION,          // 0x23
        &&INVALID_INSTRUCTION,          // 0x24
        &&INVALID_INSTRUCTION,          // 0x25
        &&INVALID_INSTRUCTION,          // 0x26
        &&INVALID_INSTRUCTION,          // 0x27
        &&INVALID_INSTRUCTION,          // 0x28
        &&INVALID_INSTRUCTION,          // 0x29
        &&INVALID_INSTRUCTION,          // 0x2A
        &&INVALID_INSTRUCTION,          // 0x2B
        &&INVALID_INSTRUCTION,          // 0x2C
        &&INVALID_INSTRUCTION,          // 0x2D
        &&INVALID_INSTRUCTION,          // 0x2E
        &&INVALID_INSTRUCTION,          // 0x2F
        &&INVALID_INSTRUCTION,          // 0x30
        &&INVALID_INSTRUCTION,          // 0x31
        &&INVALID_INSTRUCTION,          // 0x32
        &&INVALID_INSTRUCTION,          // 0x33
        &&INVALID_INSTRUCTION,          // 0x34
        &&INVALID_INSTRUCTION,          // 0x35
        &&INVALID_INSTRUCTION,          // 0x36
        &&INVALID_INSTRUCTION,          // 0x37
        &&INVALID_INSTRUCTION,          // 0x38
        &&INVALID_INSTRUCTION,          // 0x39
        &&INVALID_INSTRUCTION,          // 0x3A
        &&INVALID_INSTRUCTION,          // 0x3B
        &&INVALID_INSTRUCTION,          // 0x3C
        &&INVALID_INSTRUCTION,          // 0x3D
        &&INVALID_INSTRUCTION,          // 0x3E
        &&INVALID_INSTRUCTION,          // 0x3F
    };
    
    static void* regimmTable[32] {
        &&BLTZ,     // 0x00
        &&BGEZ,     // 0x01
        &&BLTZL,    // 0x02
        &&BGEZL,    // 0x03
        &&INVALID_INSTRUCTION,          // 0x04
        &&INVALID_INSTRUCTION,          // 0x05
        &&INVALID_INSTRUCTION,          // 0x06
        &&INVALID_INSTRUCTION,          // 0x07
        &&TGEI,     // 0x08
        &&TGEIU,    // 0x09
        &&TLTI,     // 0x0A
        &&TLTIU,    // 0x0B
        &&TEQI,     // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&TNEI,     // 0x0E
        &&INVALID_INSTRUCTION,          // 0x0F
        &&BLTZAL,   // 0x10
        &&BGEZAL,   // 0x11
        &&BLTZALL,  // 0x12
        &&BGEZALL,  // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
    };
    
    static void* cop0Table[32] {
        &&MFC0,     // 0x00
        &&INVALID_INSTRUCTION,          // 0x01
        &&INVALID_INSTRUCTION,          // 0x02
        &&INVALID_INSTRUCTION,          // 0x03
        &&MTC0,     // 0x04
        &&INVALID_INSTRUCTION,          // 0x05
        &&INVALID_INSTRUCTION,          // 0x06
        &&INVALID_INSTRUCTION,          // 0x07
        &&INVALID_INSTRUCTION,          // 0x08
        &&INVALID_INSTRUCTION,          // 0x09
        &&INVALID_INSTRUCTION,          // 0x0A
        &&DI,       // 0x0B
        &&INVALID_INSTRUCTION,          // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
        &&INVALID_INSTRUCTION,          // 0x0F
        &&INVALID_INSTRUCTION,          // 0x10
        &&INVALID_INSTRUCTION,          // 0x11
        &&INVALID_INSTRUCTION,          // 0x12
        &&INVALID_INSTRUCTION,          // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&ERET,     // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&DERET,    // 0x1F
    };
    
    static void* cop0COTable[32] {
        &&INVALID_INSTRUCTION,          // 0x00
        &&INVALID_INSTRUCTION,          // 0x01
        &&INVALID_INSTRUCTION,          // 0x02
        &&INVALID_INSTRUCTION,          // 0x03
        &&INVALID_INSTRUCTION,          // 0x04
        &&INVALID_INSTRUCTION,          // 0x05
        &&INVALID_INSTRUCTION,          // 0x06
        &&INVALID_INSTRUCTION,          // 0x07
        &&INVALID_INSTRUCTION,          // 0x08
        &&INVALID_INSTRUCTION,          // 0x09
        &&INVALID_INSTRUCTION,          // 0x0A
        &&INVALID_INSTRUCTION,          // 0x0B
        &&INVALID_INSTRUCTION,          // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
        &&INVALID_INSTRUCTION,          // 0x0F
        &&INVALID_INSTRUCTION,          // 0x10
        &&INVALID_INSTRUCTION,          // 0x11
        &&INVALID_INSTRUCTION,          // 0x12
        &&INVALID_INSTRUCTION,          // 0x13
        &&INVALID_INSTRUCTION,          // 0x14
        &&INVALID_INSTRUCTION,          // 0x15
        &&INVALID_INSTRUCTION,          // 0x16
        &&INVALID_INSTRUCTION,          // 0x17
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
    };
    
    // Begin Dispatch Loop
dispatchStart:
    try {
        
    #ifdef TEST_PROJECT
        fetch() DECODE_OPCODE(); goto *opcodeTable[opcode];
    #else
        DISPATCH();
    #endif
/*
 *  === Opcode Instructions ===
 */
    // 0x00 Funct/SPECIAL Instructions
    FUNCT:
        DECODE_FUNCT();
        goto *functTable[funct];
    
    // 0x01 REGIMM Instructions
    REGIMM:
        DECODE_RT();
        goto *regimmTable[rt];
    
    // 0x02 Jump
    J:
        DECODE_JIMM();
        branchDelay = true;
        jimm <<= 2;
        branchAddr = PC & 0xF0000000;
        branchAddr |= jimm;
        DISPATCH();
    
    // 0x03 Jump And Link
    JAL:
        DECODE_JIMM();
        registers[31] = PC+4;
        branchDelay = true;
        jimm <<= 2;
        branchAddr = PC & 0xF0000000;
        branchAddr |= jimm;
        DISPATCH();
    
    // 0x04 Branch If Equal
    BEQ:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        if (registers[rs] == registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x05 Branch on Not Equal
    BNE:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        if (registers[rs] != registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x06 Branch on Less Than or Equal to Zero
    BLEZ:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] <= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x07 Branch on Greater Than Zero
    BGTZ:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] > 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x08 Add Immediate
    ADDI:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = 0;
        if (__builtin_sadd_overflow(registers[rs], immse, &tempi32)) {
            // FIXME: Overflow trigger exception
        }
        else {
            registers[rt] = tempi32;
        }
        DISPATCH();
    
    // 0x09 Add Unsigned Immediate
    ADDIU:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        registers[rt] = registers[rs] + immse;
        DISPATCH();
    
    // 0x0A Set on Less Than Immediate
    SLTI:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        if ((int32_t)registers[rs] < immse) {
            registers[rt] = 1;
        }
        else {
            registers[rt] = 0;
        }
        DISPATCH();
        
    // 0x0B Set on Less Than Immediate Unsigned
    SLTIU:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        if (registers[rs] < (uint32_t)immse) {
            registers[rt] = 1;
        }
        else {
            registers[rt] = 0;
        }
        DISPATCH();
    
    // 0x0C And Immediate
    ANDI:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        registers[rt] = registers[rs] & imm;
        DISPATCH();
        
    // 0x0D OR Immediate
    ORI:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        registers[rt] = registers[rs] | imm;
        DISPATCH();
        
    // 0x0E XOR Immediate
    XORI:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        registers[rt] = registers[rs] ^ imm;
        DISPATCH();
        
    // 0x0F Load Upper Immediate
    LUI:
        DECODE_RT();
        DECODE_IMM();
        registers[rt] = imm << 16;
        DISPATCH();
        
    // 0x10 COP0 Instructions
    COP0:
        if (DECODE_CO() == 0) {
            DECODE_RS();
            goto *cop0Table[rs];
        }
        else {
            DECODE_FUNCT();
            
        }
    
    // 0x11 COP1
    COP1:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x12 COP2
    COP2:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    // 0x13
    
    // 0x14 Branch on Equal Likely
    BEQL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        if (registers[rs] == registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x15 Branch on Not Equal Likely
    BNEL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMM();
        if (registers[rs] != registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x16 Branch on Less Than or Equal to Zero Likely
    BLEZL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] <= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x17 Branch on Greater Than Zero Likely
    BGTZL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] > 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x18
    
    // 0x19
    
    // 0x1A
    
    // 0x1B

    // 0x1C SPECIAL2 Instructions
    SPECIAL2:
        DECODE_FUNCT();
        goto *special2Table[funct];
    
    // 0x1D
    
    // 0x1E
    
    // 0x1F SPECIAL3 Instructions
    SPECIAL3:
        DECODE_FUNCT();
        goto *special3Table[funct];
    
    // 0x20 Load Byte
    LB:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readByte(tempi32, &tempu8, &cop0_processor);
        tempi32 = tempu8;
        tempi32 <<= 24;
        tempi32 >>= 24;
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x21 Load Half
    LH:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readHalf(tempi32, &tempu16, &cop0_processor);
        tempi32 = tempu16;
        tempi32 <<= 16;
        tempi32 >>= 16;
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x22 Load Word Left
    LWL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->readHalfUnaligned(tempi32, tempi32+1, &tempu16, &cop0_processor);
        tempu32 = tempu16 << 16;
        registers[rt] &= 0x0000FFFF;
        registers[rt] |= tempu32;
        DISPATCH();
    
    // 0x23 Load Word
    LW:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readWord(tempi32, &tempu32, &cop0_processor);
        tempi32 = tempu32;
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x24 Load Byte Unsigned
    LBU:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readByte(tempi32, &tempu8, &cop0_processor);
        registers[rt] = tempu8;
        DISPATCH();
    
    // 0x25 Load Halfword Unsigned
    LHU:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readHalf(tempi32, &tempu16, &cop0_processor);
        registers[rt] = tempu16;
        DISPATCH();
    
    // 0x26 Load Word Right
    LWR:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->readHalfUnaligned(tempi32-1, tempi32, &tempu16, &cop0_processor);
        tempu32 = tempu16;
        registers[rt] &= 0xFFFF0000;
        registers[rt] |= tempu32;
        DISPATCH();

    // 0x27
    
    // 0x28 Store Byte
    SB:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->storeByte(tempi32, registers[rt], &cop0_processor);
        DISPATCH();
    
    // 0x29 Store Halfword
    SH:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->storeHalf(tempi32, registers[rt], &cop0_processor);
        DISPATCH();
    
    // 0x2A Store Word Left
    SWL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->storeHalfUnaligned(tempi32, tempi32+1, (registers[rt] & 0xFFFF0000) >> 16, &cop0_processor);
        DISPATCH();
    
    // 0x2B Store Word
    SW:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->storeWord(tempi32, registers[rt], &cop0_processor);
        DISPATCH();
    
    // 0x2C
    
    // 0x2D
    
    // 0x2E Store Word Right
    SWR:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->storeHalfUnaligned(tempi32-1, tempi32, registers[rt] & 0x0000FFFF, &cop0_processor);
        DISPATCH();
    
    // 0x2F Cache
    CACHE:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x30 Load Linked Word
    LL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x31 Load Word to Floating Point
    LWC1:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x32 Load Word to Coprocessor 2
    LWC2:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x33 Prefetch
    PREF:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x34
    
    // 0x35 Load Doubleword to Floating Point
    LDC1:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x36 Load Doubleword to Coprocessor 2
    LDC2:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x37
    
    // 0x38 Store Conditional Word
    SC:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x39 Store Word from Floating Point
    SWC1:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x3A Store Word from Coprocessor 2
    SWC2:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x3B
    
    // 0x3C
    
    // 0x3D Store Doubleword from Floating Point
    SDC1:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x3E Store Doubleword from Coprocessor 2
    SDC2:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x3F
    
/*
 * === END Opcodes ===
 */
    
/*
 *  === Func Instructions ===
 */
    
    // 0x00 Shift Left Logical
    SLL:
        DECODE_RD();
        DECODE_RT();
        DECODE_SHAMT();
        registers[rd] = registers[rt] << shamt;
        DISPATCH();
    
    // 0x01
    
    // 0x02 Shift Right Logical
    SRL:
        DECODE_RS();
        DECODE_RD();
        DECODE_RT();
        DECODE_SHAMT();
        if (rs == 0) {
            // SRL
            registers[rd] = registers[rt] >> shamt;
        }
        else {
            // ROTR: Rotate Word Right
            // https://en.wikipedia.org/wiki/Circular_shift
            tempu32 = (CHAR_BIT * sizeof(registers[rt]) - 1);
            tempu32_2 = shamt;
            tempu32_2 &= tempu32;
            registers[rd] = (registers[rt] >> tempu32_2) | (registers[rt] << ((-tempu32_2) & tempu32));
        }
        DISPATCH();
    
    // 0x03 Shift Right Arithmetic
    SRA:
        DECODE_RD();
        DECODE_RT();
        DECODE_SHAMT();
        registers[rd] = (int32_t)registers[rt] >> shamt;
        DISPATCH();
    
    // 0x04 Shift Word Left Logical Variable
    SLLV:
        DECODE_RD();
        DECODE_RT();
        DECODE_RS();
        registers[rd] = registers[rt] << registers[rs];
        DISPATCH();
    
    // 0x05
    
    // 0x06 Shift Word Right Logical Variable
    SRLV:
        DECODE_RD();
        DECODE_RT();
        DECODE_RS();
        DECODE_SHAMT();
        if (shamt == 0) {
            // SRLV
            registers[rd] = registers[rt] >> registers[rs];
        }
        else {
            // ROTRV:
            // ROTR Rotate Word Right Variable
            // https://en.wikipedia.org/wiki/Circular_shift
            tempu32 = (CHAR_BIT * sizeof(registers[rt]) - 1);
            tempu32_2 = registers[rs] & 0x0000001F;
            tempu32_2 &= tempu32;
            registers[rd] = (registers[rt] >> tempu32_2) | (registers[rt] << ((-tempu32_2) & tempu32));
        }
        DISPATCH();
    
    // 0x07 Shift Word Right Arithmetic Variable
    SRAV:
        DECODE_RD();
        DECODE_RT();
        DECODE_RS();
        registers[rd] = (int32_t)registers[rt] >> registers[rs];
        DISPATCH();
    
    // 0x08 Jump Register
    JR:
        DECODE_RS();
        // FIXME: checks Config1.CA
        branchDelay = true;
        branchAddr = registers[rs];
        DISPATCH();
    
    // 0x09 Jump And Link Register
    JALR:
        DECODE_RD();
        DECODE_RS();
        branchDelay = true;
        registers[rd] = PC+4;
        branchAddr = registers[rs];
        DISPATCH();
    
    // 0x0A Move Conditional On Zero
    MOVZ:
        DECODE_RD();
        DECODE_RT();
        DECODE_RS();
        if (registers[rt] == 0) {
            registers[rd] = registers[rs];
        }
        DISPATCH();
    
    // 0x0B Move Conditional On Not Zero
    MOVN:
        DECODE_RD();
        DECODE_RT();
        DECODE_RS();
        if (registers[rt] != 0) {
            registers[rd] = registers[rs];
        }
        DISPATCH();
    
    // 0x0C System Call
    SYSCALL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0D Breakpoint
    BREAK:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0E
    
    // 0x0F Sync
    SYNC:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x10 Move From HI Register
    MFHI:
        DECODE_RD();
        registers[rd] = HI;
        DISPATCH();
    
    // 0x11 Move To HI Register
    MTHI:
        DECODE_RS();
        HI = registers[rs];
        DISPATCH();
    
    // 0x12 Move From LO Register
    MFLO:
        DECODE_RD();
        registers[rd] = LO;
        DISPATCH();
    
    // 0x13 Move To LO Register
    MTLO:
        DECODE_RS();
        LO = registers[rs];
        DISPATCH();
    
    // 0x14
    
    // 0x15
    
    // 0x16
    
    // 0x17
    
    // 0x18 Multiply Word
    MULT:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Sign extend
        tempi64 <<= 32;
        tempi64 >>= 32;
        tempi64_2 <<= 32;
        tempi64_2 >>= 32;
        // Mult
        tempi64 = tempi64 * tempi64_2;
        HI = tempi64 >> 32;
        LO = tempi64 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x19 Multiply Unsigned Word
    MULTU:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Mult
        tempi64 = tempi64 * tempi64_2;
        HI = tempi64 >> 32;
        LO = tempi64 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x1A Divide Word
    DIV:
        DECODE_RS();
        DECODE_RT();
        if (registers[rt] != 0) {
            HI = (int32_t)registers[rs] % (int32_t)registers[rt];
            LO = (int32_t)registers[rs] / (int32_t)registers[rt];
        }
        DISPATCH();
    
    // 0x1B Divide Unsigned Word
    DIVU:
        DECODE_RS();
        DECODE_RT();
        if (registers[rt] != 0) {
            HI = registers[rs] % registers[rt];
            LO = registers[rs] / registers[rt];
        }
        DISPATCH();
    
    // 0x1C
    
    // 0x1D
    
    // 0x1E
    
    // 0x1F
    
    // 0x20 Add Word
    ADD:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        tempi32 = 0;
        if (__builtin_sadd_overflow(registers[rs], registers[rt], &tempi32)) {
            // FIXME: Overflow trigger exception
        }
        else {
            registers[rd] = tempi32;
        }
        DISPATCH();
    
    // 0x21 Add Unsigned Word
    ADDU:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = registers[rs] + registers[rt];
        DISPATCH();
    
    // 0x22 Subtract Word
    SUB:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        tempi32 = 0;
        if (__builtin_ssub_overflow(registers[rs], registers[rt], &tempi32)) {
        // FIXME: Overflow trigger exception
        }
        else {
            registers[rd] = tempi32;
        }
        DISPATCH();
    
    // 0x23 Subtract Word Unsigned
    SUBU:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = registers[rs] - registers[rt];
        DISPATCH();
    
    // 0x24 Bitwise AND
    AND:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = registers[rs] & registers[rt];
        DISPATCH();
    
    // 0x25 Bitwise OR
    OR:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = registers[rs] | registers[rt];
        DISPATCH();
    
    // 0x26 Bitwise XOR
    XOR:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = registers[rs] ^ registers[rt];
        DISPATCH();
    
    // 0x27 Bitwise NOR
    NOR:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        registers[rd] = ~(registers[rs] | registers[rt]);
        DISPATCH();
    // 0x28
    
    // 0x29
    
    // 0x2A Set on Less Than
    SLT:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        if ((int32_t)registers[rs] < (int32_t)registers[rt]) {
            registers[rd] = 1;
        }
        else {
            registers[rd] = 0;
        }
        DISPATCH();
    
    // 0x2B Set on Less Than Unsigned
    SLTU:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        if (registers[rs] < registers[rt]) {
            registers[rd] = 1;
        }
        else {
            registers[rd] = 0;
        }
        DISPATCH();
    
    // 0x2C
    
    // 0x2D
    
    // 0x2E
    
    // 0x2F
    
    // 0x30 Trap if Greater or Equal
    TGE:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x31 Trap if Greater or Equal Unsigned
    TGEU:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x32 Trap if Less Than
    TLT:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x33 Trap if Less Than Unsigned
    TLTU:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x34 Trap if Equal
    TEQ:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x35 Trap if Not Equal
    TNE:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
/*
 * === END Funcs ===
 */

/*
 *  === SPECIAL2 Instructions ===
 */
    // 0x00 Multiply and Add Word to Hi,Lo
    MADD:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Sign extend
        tempi64 <<= 32;
        tempi64 >>= 32;
        tempi64_2 <<= 32;
        tempi64_2 >>= 32;
        // Mult
        tempi64 = tempi64 * tempi64_2;
        // Load HI/LO
        tempi64_2 = HI;
        tempi64_2 <<= 32;
        tempi64_2 |= LO;
        // Add
        tempi64_2 += tempi64;
        HI = tempi64_2 >> 32;
        LO = tempi64_2 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x01 Multiply and Add Unsigned Word to Hi,Lo
    MADDU:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Mult
        tempi64 = tempi64 * tempi64_2;
        // Load HI/LO
        tempi64_2 = HI;
        tempi64_2 <<= 32;
        tempi64_2 |= LO;
        // Add
        tempi64_2 += tempi64;
        HI = tempi64_2 >> 32;
        LO = tempi64_2 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x02 Multiply Word to GPR
    MUL:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        tempi64 = (int32_t)registers[rs] * (int32_t)registers[rt];
        registers[rd] = tempi64 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x04 Multiply and Subtract Word to Hi,Lo
    MSUB:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Sign extend
        tempi64 <<= 32;
        tempi64 >>= 32;
        tempi64_2 <<= 32;
        tempi64_2 >>= 32;
        // Mult
        tempi64 = tempi64 * tempi64_2;
        // Load HI/LO
        tempi64_2 = HI;
        tempi64_2 <<= 32;
        tempi64_2 |= LO;
        // Sub
        tempi64_2 -= tempi64;
        HI = tempi64_2 >> 32;
        LO = tempi64_2 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x05 Multiply and Subtract Unsigned Word to Hi,Lo
    MSUBU:
        DECODE_RS();
        DECODE_RT();
        tempi64 = registers[rs];
        tempi64_2 = registers[rt];
        // Mult
        tempi64 = tempi64 * tempi64_2;
        // Load HI/LO
        tempi64_2 = HI;
        tempi64_2 <<= 32;
        tempi64_2 |= LO;
        // Sub
        tempi64_2 -= tempi64;
        HI = tempi64_2 >> 32;
        LO = tempi64_2 & 0x00000000FFFFFFFF;
        DISPATCH();
    
    // 0x20 Count Leading Zeroes in Word
    CLZ:
        DECODE_RS();
        DECODE_RD();
        tempu32 = 0;
        for (tempi32 = 31; tempi32 >= 0; tempi32--) {
            if ((registers[rs] & (0x1 << tempi32)) != 0) {
                tempu32 = 31 - tempi32;
                break;
            }
        }
        registers[rd] = tempu32;
        DISPATCH();
    
    // 0x21 Count Loading Ones in Word
    CLO:
        DECODE_RS();
        DECODE_RD();
        tempu32 = 0;
        for (tempi32 = 31; tempi32 >= 0; tempi32--) {
            if ((registers[rs] & (0x1 << tempi32)) == 0) {
                tempu32 = 31 - tempi32;
                break;
            }
        }
        registers[rd] = tempu32;
        DISPATCH();
    
    // 0x3F Software Debug Breakpoint
    SDBBP:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
/*
 * === END SPECIAL2 ===
 */
    
/*
 *  === SPECIAL3 Instructions ===
 */
    // 0x00 Extract Bit Field
    EXT:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        DECODE_SHAMT();
        // lsb = shamt
        // msbd = rd
        tempu16 = shamt + rd;
        if (tempu16 > 31) {
            // Unpredictable
        }
        else {
            tempu32 = 0;
            tempu32_2 = 0;
            for (;shamt <= tempu16; shamt++, tempu32_2++) {
                tempu32 |= ((registers[rs] & (0x1u << shamt)) > 0 ? 1u : 0u) << tempu32_2;
            }
            registers[rt] = tempu32;
        }
        DISPATCH();
    
    // 0x04 Insert Bit Field
    INS:
        DECODE_RS();
        DECODE_RT();
        DECODE_RD();
        DECODE_SHAMT();
        // lsb = shamt
        // msbd = rd
        if (shamt > rd) {
            // Unpredictable
        }
        else {
            tempu32 = 0;
            tempu16 = rd - shamt;
            // Extract bits
            for (tempu32_2 = 0; tempu32_2 <= tempu16; tempu32_2++) {
                tempu32 |= ((registers[rs] & (0x1u << tempu32_2)) > 0 ? 1u : 0u) << tempu32_2;
            }
            // Merge bits
            for (tempu32_2 = 0; shamt <= rd; shamt++, tempu32_2++) {
                registers[rt] &= ~(0x1 << shamt);
                registers[rt] |= ((tempu32 & (0x1 << tempu32_2)) > 0 ? 1u : 0u) << shamt;
            }
        }
        DISPATCH();
    
    // 0x20 BSHFL
    BSHFL:
        // Could be split into another table but there's only 3 instructions using this format?
        DECODE_SHAMT();
        DECODE_RT();
        DECODE_RD();
        switch (shamt) {
            // WSBH: Word Swap Bytes Within Halfwords
            case 0x02: {
                registers[rd] =  (registers[rt] & 0x00FF0000) << 8;
                registers[rd] |= (registers[rt] & 0xFF000000) >> 8;
                registers[rd] |= (registers[rt] & 0x000000FF) << 8;
                registers[rd] |= (registers[rt] & 0x0000FF00) >> 8;
                DISPATCH();
            }
            // SEB: Sign Extend Byte
            case 0x10: {
                tempi32 = registers[rt] & 0x000000FF;
                tempi32 <<= 24;
                tempi32 >>= 24;
                registers[rd] = tempi32;
                DISPATCH();
            }
            // SEH: Sign Extend Halfword
            case 0x18: {
                tempi32 = registers[rt] & 0x0000FFFF;
                tempi32 <<= 16;
                tempi32 >>= 16;
                registers[rd] = tempi32;
                DISPATCH();
            }
            default:
                goto UNIMPLEMENTED_INSTRUCTION;
        }
    
/*
 * === END SPECIAL3 ===
 */
    
/*
 *  === REGIMM Instructions ===
 */
    // 0x00 Branch on Less Than Zero
    BLTZ:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] < 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x01 Branch on Greater Than or Equal to Zero
    BGEZ:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] >= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x02 Branch on Less Than Zero Likely
    BLTZL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] < 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x03 Branch on Greater Than or Equal to Zero Likely
    BGEZL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] >= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x08 Trap if Greater or Equal Immediate
    TGEI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x09 Trap if Greater or Equal Immediate Unsigned
    TGEIU:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0A Trap if Less Than Immediate
    TLTI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0B Trap if Less Than Immediate Unsigned
    TLTIU:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0C Trap if Equal Immediate
    TEQI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x0E Trap if Not Equal
    TNEI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x10 Branch on Less Than Zero and Link
    BLTZAL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] < 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x11 Branch on Greater Than or Equal to Zero and Link
    BGEZAL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] >= 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x12 Branch on Less Than Zero and Link Likely
    BLTZALL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] < 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x13 Branch on Greater Than Zero and Link Likely
    BGEZALL:
        DECODE_RS();
        DECODE_IMM();
        if ((int32_t)registers[rs] >= 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
/*
 * === END REGIMM ===
 */
    
/*
 *  === COP0 Instructions ===
 */
    // 0x00 Move From Coprocessor0
    MFC0:
        DECODE_SEL();
        DECODE_RT();
        DECODE_RD();
        registers[rt] = cop0_processor.getRegister(rd, sel);
        DISPATCH();
    
    // 0x04 Move To Coprocessor0
    MTC0:
        DECODE_SEL();
        DECODE_RT();
        DECODE_RD();
        cop0_processor.setRegister(rd, sel, registers[rt]);
        DISPATCH();
    
    // 0x0B Disable Interrupts
    DI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
        
    // 0x18 Exception Return
    ERET:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x1F Debug Exception Return
    DERET:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
/*
 * === END COP0 ===
 */
    
    // Miscellaneous
    HANDLE_INTERRUPT:
        //goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    HANDLE_TRAP:
    
    INVALID_INSTRUCTION:
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg("ERROR: Invalid instruction!");
            debugPrint();
        }
        return;
    
    UNIMPLEMENTED_INSTRUCTION:
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg("ERROR: Unimplemented instruction!");
            debugPrint();
        }
        return;
    
    CPU_HALT:
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg("CPU Halted.");
            debugPrint();
        }
        return;
        
    }
    // Multi-catch for in processor interrupts
    // exceptions and simulation errors
    catch (std::runtime_error& e) {
    #ifdef TEST_PROJECT
        exceptRestartLoop = false;
    #else
        exceptRestartLoop = true;
    #endif
        
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg(e.what());
        }
    }
    
    // ISO C++ forbids gotos into a try-catch block
    // however gotos out of a try-catch and within a try
    // are valid so for catch handlers that restart
    // execution (i.e. not simulation errors)
    // this restarts dispatching
    if (exceptRestartLoop) {
        goto dispatchStart;
    }
}

