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
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "lb", "lh", "lwl", "lw", "lbu", "lhu", "lwr", "invalid", "sb", "sh",
/* 0x2A */ "swl", "sw", "invalid", "invalid", "swr", "cache",
/* 0x30 */ "ll", "lwcl", "invalid", "pref", "invalid", "invalid", "invalid", "invalid", "sc", "swcl",
/* 0x3A */ "invalid", "invalid", "invalid", "sdc1", "sdc2", "invalid"
};

const char* CPU::functNames[64] {
/* 0x00 */ "sll", "invalid", "srl", "sra", "sllv", "srlv", "srav", "jr", "jalr",
/* 0x0A */ "movz", "movn", "syscall", "break", "invalid", "sync",
/* 0x10 */ "mfhi", "mthi", "mflo", "mtlo", "invalid", "invalid", "invalid", "invalid", "mult", "multu",
/* 0x1A */ "div", "divu", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "add", "addu", "sub", "subu", "and", "or", "xor", "nor", "invalid", "invalid",
/* 0x2A */ "slt", "sltu", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "tge", "tgeu", "tlt", "tltu", "teq", "invalid", "tne", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::special2Names[64] {
/* 0x00 */ "invalid", "invalid", "mul", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "clz", "clo", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x2A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::special3Names[64] {
/* 0x00 */ "ext", "invalid", "invalid", "invalid", "ins", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x20 */ "seh", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x2A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x30 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x3A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::regimmNames[32] {
/* 0x00 */ "bltz", "bgez", "bltzl", "bgezl", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "bltzal", "bgezal", "bltzall", "bgezall", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::cop0Names[32] {
/* 0x00 */ "mfc0", "invalid", "invalid", "invalid", "mtc0", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x0A */ "invalid", "di", "invalid", "invalid", "invalid", "invalid",
/* 0x10 */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid",
/* 0x1A */ "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
};

const char* CPU::registerNames[32] {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

// Parameterized Constructor
// Attaches the CPU to other devices
CPU::CPU(PMMU* memory) : memory(memory) {
}

// Sets the initial program counter for the CPU
void CPU::setPC(uint32_t addr) {
    PC = addr;
}

// Public starting function for the CPU
void CPU::start() {
    dispatchLoop();
}

// Prints information about the cpu's execution per cycle
// Also does on the fly disassembly
#ifdef DEBUGCPU
void CPU::debugPrint() {
    std::cout << "=== Decode ===" << std::endl
    << std::hex << "PC = 0x" << PC-4 << std::endl
    << "IR = 0x" << IR << std::endl
    << "opcode = 0x" << (uint16_t)opcode << std::endl << std::dec
    << "rs = " << (uint16_t)rs << std::endl
    << "rt = " << (uint16_t)rt << std::endl
    << "rd = " << (uint16_t)rd << std::endl
    << "shamt = " << (int16_t)shamt << std::endl
    << "funct = " << (uint16_t)funct << std::endl
    << "imm = " << (int16_t)imm << std::endl
    << "jimm = 0x" << std::hex << (int)jimm << std::endl
    << "=== END Decode ===" << std::endl;
    std::cout << std::dec << "=== Disassembly ===" << std::endl;
    if (branchDelay) {
        std::cout << "-- Delay Slot --" << std::endl;
    }
    switch (opcode) {
        case 0x0: {
            std::cout << "SPECIAL RD RS RT" << std::endl
            << functNames[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        case 0x1: {
            std::cout << "REGIMM RS IMM" << std::endl
            << regimmNames[rt] << " " << registerNames[rs] << ", " << imm << std::endl;
            break;
        }
        case 0x10: {
            std::cout << "COP0 RT RD SEL" << std::endl
            << cop0Names[rs] << " " << registerNames[rt] << ", " << registerNames[rd] << ", " << (imm&0x7) << std::endl;
            break;
        }
        case 0x1C: {
            std::cout << "SPECIAL2 RD RS RT" << std::endl
            << special2Names[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        case 0x1F: {
            std::cout << "SPECIAL3 RD RS RT" << std::endl
            << special3Names[funct] << " " << registerNames[rd] << ", " << registerNames[rs] << ", " << registerNames[rt] << std::endl;
            break;
        }
        default: {
            std::cout << "OPCODE RT RS IMM/JIMM" << std::endl
            << opcodeNames[opcode] << " " << registerNames[rt] << ", " << registerNames[rs] << ", " << imm << std::hex << ", 0x" << jimm << std::endl;
            break;
        }
    }
    std::cout << "=== END Disassembly ===" << std::endl << std::endl;

}
#endif

// Fetches the next instruction into the program counter and instruction register
__attribute__((always_inline)) void CPU::fetch() {
    // Reset r0
    registers[0] = 0;
    
    // Check if we need to branch
    if (branch) {
        branch = false;
        branchDelay = false;
        PC = branchAddr;
        IR = memory->readWord(PC);
        PC += 4;
    }
    else {
        IR = memory->readWord(PC);
        PC += 4;
    }
    
    // Check if we're in a delay slot so we branch next time
    if (branchDelay) {
        branch = true;
    }
}

// Decodes the word in IR into its component parts
__attribute__((always_inline)) void CPU::decode() {
    opcode = (IR & OPCODEMASK) >> OPCODESHIFT;
    rs = (IR & RSMASK) >> RSSHIFT;
    rt = (IR & RTMASK) >> RTSHIFT;
    rd = (IR & RDMASK) >> RDSHIFT;
    shamt = (IR & SHAMTMASK) >> SHAMTSHIFT;
    funct = (IR & FUNCTMASK) >> FUNCTSHIFT;
    imm = (IR & IMMMASK) >> IMMSHIFT;
    jimm = (IR & JIMMMASK) >> JIMMSHIFT;
    
    #ifdef DEBUGCPU
    debugPrint();
    #endif
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
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x11
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x12
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
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x31
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x32
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x33
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x34
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x35
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x36
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x37
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x38
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x39
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3A
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3B
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3C
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3D
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3E
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
        &&INVALID_INSTRUCTION,          // 0x00
        &&INVALID_INSTRUCTION,          // 0x01
        &&MUL,      // 0x02
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
        &&INVALID_INSTRUCTION,          // 0x3F
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
        &&SEH,      // 0x20
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
        &&INVALID_INSTRUCTION,          // 0x08
        &&INVALID_INSTRUCTION,          // 0x09
        &&INVALID_INSTRUCTION,          // 0x0A
        &&INVALID_INSTRUCTION,          // 0x0B
        &&INVALID_INSTRUCTION,          // 0x0C
        &&INVALID_INSTRUCTION,          // 0x0D
        &&INVALID_INSTRUCTION,          // 0x0E
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
        &&INVALID_INSTRUCTION,          // 0x18
        &&INVALID_INSTRUCTION,          // 0x19
        &&INVALID_INSTRUCTION,          // 0x1A
        &&INVALID_INSTRUCTION,          // 0x1B
        &&INVALID_INSTRUCTION,          // 0x1C
        &&INVALID_INSTRUCTION,          // 0x1D
        &&INVALID_INSTRUCTION,          // 0x1E
        &&INVALID_INSTRUCTION,          // 0x1F
    };
    
    // Clang invalid error workaround
    //silence: void* silenceptr = &&silence;
    
    
    
    // Dispatch macro
    #define DISPATCH() fetch(); decode(); goto *opcodeTable[opcode]
    
    // Begin Dispatch Loop
    DISPATCH();
    
/*
 *  === Opcode Instructions ===
 */
    // 0x00 Funct/SPECIAL Instructions
    FUNCT:
        goto *functTable[funct];
    
    // 0x01 REGIMM Instructions
    REGIMM:
        goto *regimmTable[rt];
    
    // 0x02 Jump
    J:
        branchDelay = true;
        jimm <<= 2;
        branchAddr = PC & 0xF0000000;
        branchAddr |= jimm;
        DISPATCH();
    
    // 0x03 Jump And Link
    JAL:
        registers[31] = PC+4;
        branchDelay = true;
        jimm <<= 2;
        branchAddr = PC & 0xF0000000;
        branchAddr |= jimm;
        DISPATCH();
    
    // 0x04 Branch If Equal
    BEQ:
        if (registers[rs] == registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x05 Branch on Not Equal
    BNE:
        if (registers[rs] != registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x06 Branch on Less Than or Equal to Zero
    BLEZ:
        if (registers[rs] <= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x07 Branch on Greater Than Zero
    BGTZ:
        if (registers[rs] > 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x08 Add Immediate
    ADDI:
        tempi32 = 0;
        if (__builtin_sadd_overflow(registers[rs], (int16_t)imm, &tempi32)) {
            // FIXME: Overflow trigger exception
        }
        else {
            registers[rt] = tempi32;
        }
        DISPATCH();
    
    // 0x09 Add Unsigned Immediate
    ADDIU:
        registers[rt] = registers[rs] + (int16_t)imm;
        DISPATCH();
    
    // 0x10 COP0 Instructions
    COP0:
        goto *cop0Table[rs];
    
    // 0x1C SPECIAL2 Instructions
    SPECIAL2:
        goto *special2Table[funct];
    
    // 0x1F SPECIAL3 Instructions
    SPECIAL3:
        goto *special3Table[funct];
    
    // 0x0A Set on Less Than Immediate
    SLTI:
        registers[rt] = (int16_t)registers[rs] < (int16_t)imm;
        DISPATCH();
    
    // 0x0B Shift Left Immediate Unsigned
    SLTIU:
        registers[rt] = registers[rs] < imm;
        DISPATCH();
    
    // 0x0C And Immediate
    ANDI:
        registers[rt] = registers[rs] & imm;
        DISPATCH();
    
    // 0x0D OR Immediate
    ORI:
        registers[rt] = registers[rs] | imm;
        DISPATCH();
    
    // 0x0E XOR Immediate
    XORI:
        registers[rt] = registers[rs] ^ imm;
        DISPATCH();
    
    // 0x0F Load Upper Immediate
    LUI:
        registers[rt] = imm << 16;
        DISPATCH();
    // 0x10
    
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
        if (registers[rs] == registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x15 Branch on Not Equal Likely
    BNEL:
        if (registers[rs] != registers[rt]) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x16 Branch on Less Than or Equal to Zero Likely
    BLEZL:
        if (registers[rs] <= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x17 Branch on Greater Than Zero Likely
    BGTZL:
        if (registers[rs] > 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    
    // 0x18
    
    // 0x19
    
    // 0x20 Load Byte
    LB:
        tempi32 = imm;
        tempi32 += registers[rs];
        tempi32 = memory->readByte(tempi32);
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x21 Load Half
    LH:
        tempi32 = imm;
        tempi32 += registers[rs];
        tempi32 = memory->readHalf(tempi32);
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x22 Load Word Left
    LWL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    // 0x23 Load Word
    LW:
        tempi32 = imm;
        tempi32 += registers[rs];
        tempi32 = memory->readWord(tempi32);
        registers[rt] = tempi32;
        DISPATCH();
    
    // 0x24 Load Byte Unsigned
    LBU:
        tempi32 = imm;
        tempi32 += registers[rs];
        registers[rt] = memory->readByte(tempi32);
        DISPATCH();
    
    // 0x25 Load Halfword Unsigned
    LHU:
        tempi32 = imm;
        tempi32 += registers[rs];
        registers[rt] = memory->readHalf(tempi32);
        DISPATCH();
    
    // 0x26 Load Word Right
    LWR:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x27
    
    // 0x28 Store Byte
    SB:
        tempi32 = imm;
        tempi32 += registers[rs];
        memory->storeByte(tempi32, registers[rt]);
        DISPATCH();
    
    // 0x29 Store Halfword
    SH:
        tempi32 = imm;
        tempi32 += registers[rs];
        memory->storeHalf(tempi32, registers[rt]);
        DISPATCH();
    
    // 0x2A Store Word Left
    SWL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x2B Store Word
    SW:
        tempi32 = imm;
        tempi32 += registers[rs];
        memory->storeWord(tempi32, registers[rt]);
        DISPATCH();
    
    // 0x2C
    
    // 0x2D
    
    // 0x2E Store Word Right
    SWR:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x2F Cache
    CACHE:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x30 Load Linked Word
    LL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x31
    
    // 0x32
    
    // 0x33 Prefetch
    PREF:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x34
    
    // 0x35
    
    // 0x36
    
    // 0x37
    
    // 0x38 Store Conditional Word
    SC:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    // 0x39
    SWCL:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
/*
 * === END Opcodes ===
 */
    
/*
 *  === Func Instructions ===
 */
    
    // 0x00 Shift Left Logical
    SLL:
        registers[rd] = registers[rt] << shamt;
        DISPATCH();
    // 0x01
    
    // 0x02 Shift Right Logical
    SRL:
        registers[rd] = registers[rt] >> shamt;
        DISPATCH();
    // 0x03 Shift Right Arithmetic
    SRA:
        registers[rd] = (int32_t)registers[rt] >> shamt;
        DISPATCH();
    // 0x04 Shift Word Left Logical Variable
    SLLV:
        registers[rd] = registers[rt] << registers[rs];
        DISPATCH();
    // 0x05
    
    // 0x06 Shift Word Left Arithmetic Variable
    SRLV:
        registers[rd] = (int32_t)registers[rt] << registers[rs];
        DISPATCH();
    // 0x07 Shift Word Right Arithmetic Variable
    SRAV:
        registers[rd] = (int32_t)registers[rt] >> registers[rs];
        DISPATCH();
    // 0x08 Jump Register
    JR:
        // FIXME: checks Config1.CA
        branchDelay = true;
        branchAddr = registers[rs];
        DISPATCH();
    // 0x09 Jump And Link Register
    JALR:
        branchDelay = true;
        registers[rd] = PC+4;
        branchAddr = registers[rs];
        DISPATCH();
    // 0x0A Move Conditional On Zero
    MOVZ:
        if (registers[rt] == 0) {
            registers[rd] = registers[rs];
        }
        DISPATCH();
    // 0x0B Move Conditional On Not Zero
    MOVN:
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
        registers[rd] = HI;
        DISPATCH();
    // 0x11 Move To HI Register
    MTHI:
        HI = registers[rs];
        DISPATCH();
    // 0x12 Move From LO Register
    MFLO:
        registers[rd] = LO;
        DISPATCH();
    // 0x13 Move To LO Register
    MTLO:
        LO = registers[rs];
        DISPATCH();
    // 0x14
    
    // 0x15
    
    // 0x16
    
    // 0x17
    
    // 0x18 Multiply Word
    MULT:
        tempu64 = (int32_t)registers[rs] * (int32_t)registers[rt];
        HI = tempu64 >> 32;
        LO = tempu64 & 0x00000000FFFFFFFF;
        DISPATCH();
    // 0x19 Multiply Unsigned Word
    MULTU:
        tempu64 = registers[rs] * registers[rt];
        HI = tempu64 >> 32;
        LO = tempu64 & 0x00000000FFFFFFFF;
        DISPATCH();
    // 0x1A Divide Word
    DIV:
        if (registers[rt] != 0) {
            HI = (int32_t)registers[rs] % (int32_t)registers[rt];
            LO = (int32_t)registers[rs] / (int32_t)registers[rt];
        }
        DISPATCH();
    // 0x1B Divide Unsigned Word
    DIVU:
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
        registers[rd] = registers[rs] + registers[rt];
        DISPATCH();
    // 0x22 Subtract Word
    SUB:
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
        registers[rd] = registers[rs] - registers[rt];
        DISPATCH();
    // 0x24 Bitwise AND
    AND:
        registers[rd] = registers[rs] & registers[rt];
        DISPATCH();
    // 0x25 Bitwise OR
    OR:
        registers[rd] = registers[rs] | registers[rt];
        DISPATCH();
    // 0x26 Bitwise XOR
    XOR:
        registers[rd] = registers[rs] ^ registers[rt];
        DISPATCH();
    // 0x27 Bitwise NOR
    NOR:
        registers[rd] = ~(registers[rs] | registers[rt]);
        DISPATCH();
    // 0x28
    
    // 0x29
    
    // 0x2A Set on Less Than
    SLT:
        registers[rd] = (int32_t)registers[rs] < (int32_t)registers[rt];
        DISPATCH();
    // 0x2B Set on Less Than Unsigned
    SLTU:
        registers[rd] = registers[rs] < registers[rt];
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
    // 0x02 Multiply Word to GPR
    MUL:
        DISPATCH();
    
    // 0x20 Count Leading Zeroes in Word
    CLZ:
        DISPATCH();
    
    // 0x21 Count Loading Ones in Word
    CLO:
        DISPATCH();
    
/*
 * === END SPECIAL2 ===
 */
    
/*
 *  === SPECIAL3 Instructions ===
 */
    // 0x00 Extract Bit Field
    EXT:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x04 Insert Bit Field
    INS:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    
    // 0x1F Sign-Extend Halfword
    SEH:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
/*
 * === END SPECIAL3 ===
 */
    
/*
 *  === REGIMM Instructions ===
 */
    // 0x00 Branch on Less Than Zero
    BLTZ:
        if ((int32_t)registers[rs] < 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x01 Branch on Greater Than or Equal to Zero
    BGEZ:
        if ((int32_t)registers[rs] >= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x02 Branch on Less Than Zero Likely
    BLTZL:
        if ((int32_t)registers[rs] < 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x03 Branch on Greater Than or Equal to Zero Likely
    BGEZL:
        if ((int32_t)registers[rs] >= 0) {
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x10 Branch on Less Than Zero and Link
    BLTZAL:
        if ((int32_t)registers[rs] < 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x11 Branch on Greater Than or Equal to Zero and Link
    BGEZAL:
        if ((int32_t)registers[rs] >= 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x12 Branch on Less Than Zero and Link Likely
    BLTZALL:
        if ((int32_t)registers[rs] < 0) {
            registers[31] = PC+4;
            branchDelay = true;
            branchAddr = PC;
            branchAddr += (int16_t)imm << 2;
        }
        DISPATCH();
    // 0x13 Branch on Greater Than Zero and Link Likely
    BGEZALL:
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
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    // 0x04 Move To Coprocessor0
    MTC0:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
    // 0x0B Disable Interrupts
    DI:
        goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
/*
 * === END COP0 ===
 */
    
    // Miscellaneous
    HANDLE_INTERRUPT:
        //goto UNIMPLEMENTED_INSTRUCTION;
        //DISPATCH();
        
    INVALID_INSTRUCTION:
        std::cerr << "ERROR: Invalid instruction!" << std::endl;
        return;
    UNIMPLEMENTED_INSTRUCTION:
        std::cerr << "ERROR: Unimplemented instruction!" << std::endl;
        return;
}

