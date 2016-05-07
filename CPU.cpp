//
//  CPU.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "CPU.h"
#include "MIPSException.h"

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
CPU::CPU(uint8_t cpuNum, ConsoleUI* conUI, PMMU* memory) : CPUNUM(cpuNum), consoleUI(conUI), memory(memory), cycleCounter(0), signal(CONTINUE), singleStep(false), LLBit(false), exceptRestartLoop(false) {
    for (int i=0; i<32; i++) {
        registers[i] = 0;
    }
}

CPU::CPU(uint8_t cpuNum, PMMU* memory) : CPUNUM(cpuNum), consoleUI(nullptr), memory(memory), cycleCounter(0), signal(CONTINUE), singleStep(false), LLBit(false), exceptRestartLoop(false) {
    for (int i=0; i<32; i++) {
        registers[i] = 0;
    }
}

// Sets the initial program counter for the CPU
void CPU::setPC(uint32_t addr) {
    PC = addr;
}

void CPU::setRegister(uint8_t num, uint32_t val) {
    this->registers[num] = val;
}

// Public starting function for the CPU
void CPU::start() {
    dispatchLoop();
}

// Public function to signal the cpu thread
bool CPU::sendInterrupt(MIPSInterrupt interrupt) {
    // Check if interrupts enabled
    // Status_ie = 1, Status_exl = 0, Status_erl = 0
    if (!cop0.interruptsEnabled()) {
        return false;
    }
    
    // Assert interrupt line
    switch (interrupt) {
        case MIPSInterrupt::HW0: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM2) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP2);
                return true;
            }
            break;
        }
        case MIPSInterrupt::HW1: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM3) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP3);
                return true;
            }
            break;
        }
        case MIPSInterrupt::HW2: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM4) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP4);
                return true;
            }
            break;
        }
        case MIPSInterrupt::HW3: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM5) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP5);
                return true;
            }
            break;
        }
        case MIPSInterrupt::HW4: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM6) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP6);
                return true;
            }
            break;
        }
        case MIPSInterrupt::HW5: {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_IM7) > 0) {
                // Update Cause
                cop0.orRegisterHW(CO0_CAUSE, CAUSE_IP7);
                return true;
            }
            break;
        }
        default: {
            throw std::runtime_error("Invalid hardware interrupt number specified");
        }
    }
    return false;
}

void CPU::clearInterrupt(MIPSInterrupt interrupt) {
    // Assert interrupt line
    switch (interrupt) {
        case MIPSInterrupt::SW0: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP0);
            break;
        }
        case MIPSInterrupt::SW1: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP1);
            break;
        }
        case MIPSInterrupt::HW0: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP2);
            break;
        }
        case MIPSInterrupt::HW1: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP3);
            break;
        }
        case MIPSInterrupt::HW2: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP4);
            break;
        }
        case MIPSInterrupt::HW3: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP5);
            break;
        }
        case MIPSInterrupt::HW4: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP6);
            break;
        }
        case MIPSInterrupt::HW5: {
            cop0.andRegisterHW(CO0_CAUSE, ~CAUSE_IP7);
            break;
        }
        default: {
            throw std::runtime_error("Invalid hardware interrupt number specified");
        }
    }
}

// Returns the control coprocessor associated with this CPU
// Mainly for loading elf files into memory
Coprocessor0* CPU::getControlCoprocessor() {
    return &cop0;
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
bool CPU::getLLBit() {
    return this->LLBit;
}
uint32_t CPU::getLLPFN() {
    return this->LLPFN;
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
void CPU::setLLBit(bool val) {
    this->LLBit = val;
}
void CPU::setLLPFN(uint32_t val) {
    this->LLPFN = val;
}
void CPU::executeException() {
    DECODE_JIMM();
    switch (jimm) {
        case 0x00: throw ColdResetException();
        case 0x01: throw SoftResetException();
        case 0x02: throw NonmaskableInterruptException();
        case 0x03: throw MachineCheckException();
        case 0x04: throw WatchIFException();
        case 0x05: throw AddressErrorIFException();
        case 0x06: throw TLBRefillIFException();
        case 0x07: throw TLBInvalidIFException();
        case 0x08: throw TLBExecuteInhibitException();
        case 0x09: throw CacheErrorIFException();
        case 0x0A: throw BusErrorIFException();
        case 0x0B: throw CoprocessorUnusableException(CoprocessorUnusableException::CO2);
        case 0x0C: throw ReservedInstructionException();
        case 0x0D: throw IntegerOverflowException();
        case 0x0E: throw TrapException();
        case 0x0F: throw SystemCallException();
        case 0x10: throw BreakpointException();
        case 0x11: throw FloatingPointException();
        case 0x12: throw Coprocessor2Exception();
        case 0x13: throw WatchDataException();
        case 0x14: throw AddressErrorDataException();
        case 0x15: throw TLBRefillDataException(true);
        case 0x16: throw TLBInvalidDataException(true);
        case 0x17: throw TLBReadInhibitException();
        case 0x18: throw TLBModifiedException();
        case 0x19: throw CacheErrorDataException();
        case 0x1A: throw BusErrorDataException();
        case 0x1B: throw InterruptException();
        default: throw std::runtime_error("Invalid opcode for exception testing");
    }
}
#endif

// Prints information about the cpu's execution per cycle
// Also does on the fly disassembly
std::string CPU::debugPrint() {
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
            << cop0Names[rs] << " " << registerNames[rt] << ", " << registerNames[rd] << ", " << sel << std::endl;
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
    ss << "=== END Disassembly ===" << std::endl;
    return ss.str();
}

// Prints out GPR information
std::string CPU::printRegisters() {
    std::stringstream ss;
    for (uint32_t i=0; i<32; i++) {
        ss << registerNames[i] << " (" << std::dec << i << ") 0x" << std::hex << registers[i] << std::endl;
    }
    ss << std::hex << "HI 0x" << HI << std::endl;
    ss << std::hex << "LO 0x" << LO << std::endl;
    return ss.str();
}

// Check if a thread signal was received
#define checkSignal() \
    if (signal > 0) { \
        goto HANDLE_SIGNAL; \
    } \

// Checks if there is a pending interrupt
// If so then do some further processing
#define checkForInts()  \
    if ((cop0.getRegister(CO0_CAUSE) & CAUSE_INTS) > 0) { \
        goto HANDLE_INTERRUPT; \
    } \

// Does some processing to ensure the ISA representation remains consistent to software (e.g. r0 = 0)
#define updateISARep()  \
    cycleCounter++; \
\
    registers[0] = 0; \

// Fetches the next instruction into the program counter and instruction register
#define fetch()  \
    if (branch) { \
        branch = false; \
        branchDelay = false; \
        PC = branchAddr; \
    } \
\
    memory->readWordIF(PC, &IR, CPUNUM, &cop0); \
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
        &&COP1X,    // 0x13
        &&BEQL,     // 0x14
        &&BNEL,     // 0x15
        &&BLEZL,    // 0x16
        &&BGTZL,    // 0x17
        &&RESERVED_INSTRUCTION,          // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&SPECIAL2, // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
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
#ifdef TEST_PROJECT
        &&TRIGGER_EXCEPTION,            // 0x3F
#else
        &&UNIMPLEMENTED_INSTRUCTION,    // 0x3F
#endif
    };
    
    static void* functTable[64] = {
        &&SLL,      // 0x00
        &&RESERVED_INSTRUCTION,          // 0x01
        &&SRL,      // 0x02
        &&SRA,      // 0x03
        &&SLLV,     // 0x04
        &&RESERVED_INSTRUCTION,          // 0x05
        &&SRLV,     // 0x06
        &&SRAV,     // 0x07
        &&JR,       // 0x08
        &&JALR,     // 0x09
        &&MOVZ,     // 0x0A
        &&MOVN,     // 0x0B
        &&SYSCALL,  // 0x0C
        &&BREAK,    // 0x0D
        &&RESERVED_INSTRUCTION,          // 0x0E
        &&SYNC,     // 0x0F
        &&MFHI,     // 0x10
        &&MTHI,     // 0x11
        &&MFLO,     // 0x12
        &&MTLO,     // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&MULT,     // 0x18
        &&MULTU,    // 0x19
        &&DIV,      // 0x1A
        &&DIVU,     // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&RESERVED_INSTRUCTION,          // 0x1F
        &&ADD,      // 0x20
        &&ADDU,     // 0x21
        &&SUB,      // 0x22
        &&SUBU,     // 0x23
        &&AND,      // 0x24
        &&OR,       // 0x25
        &&XOR,      // 0x26
        &&NOR,      // 0x27
        &&RESERVED_INSTRUCTION,          // 0x28
        &&RESERVED_INSTRUCTION,          // 0x29
        &&SLT,      // 0x2A
        &&SLTU,     // 0x2B
        &&RESERVED_INSTRUCTION,          // 0x2C
        &&RESERVED_INSTRUCTION,          // 0x2D
        &&RESERVED_INSTRUCTION,          // 0x2E
        &&RESERVED_INSTRUCTION,          // 0x2F
        &&TGE,      // 0x30
        &&TGEU,     // 0x31
        &&TLT,      // 0x32
        &&TLTU,     // 0x33
        &&TEQ,      // 0x34
        &&RESERVED_INSTRUCTION,          // 0x35
        &&TNE,      // 0x36
        &&RESERVED_INSTRUCTION,          // 0x37
        &&RESERVED_INSTRUCTION,          // 0x38
        &&RESERVED_INSTRUCTION,          // 0x39
        &&RESERVED_INSTRUCTION,          // 0x3A
        &&RESERVED_INSTRUCTION,          // 0x3B
        &&RESERVED_INSTRUCTION,          // 0x3C
        &&RESERVED_INSTRUCTION,          // 0x3D
        &&RESERVED_INSTRUCTION,          // 0x3E
        &&RESERVED_INSTRUCTION,          // 0x3F
    };
    
    static void* special2Table[64] {
        &&MADD,     // 0x00
        &&MADDU,    // 0x01
        &&MUL,      // 0x02
        &&RESERVED_INSTRUCTION,          // 0x03
        &&MSUB,     // 0x04
        &&MSUBU,    // 0x05
        &&RESERVED_INSTRUCTION,          // 0x06
        &&RESERVED_INSTRUCTION,          // 0x07
        &&RESERVED_INSTRUCTION,          // 0x08
        &&RESERVED_INSTRUCTION,          // 0x09
        &&RESERVED_INSTRUCTION,          // 0x0A
        &&RESERVED_INSTRUCTION,          // 0x0B
        &&RESERVED_INSTRUCTION,          // 0x0C
        &&RESERVED_INSTRUCTION,          // 0x0D
        &&RESERVED_INSTRUCTION,          // 0x0E
        &&RESERVED_INSTRUCTION,          // 0x0F
        &&RESERVED_INSTRUCTION,          // 0x10
        &&RESERVED_INSTRUCTION,          // 0x11
        &&RESERVED_INSTRUCTION,          // 0x12
        &&RESERVED_INSTRUCTION,          // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&RESERVED_INSTRUCTION,          // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&RESERVED_INSTRUCTION,          // 0x1F
        &&CLZ,      // 0x20
        &&CLO,      // 0x21
        &&RESERVED_INSTRUCTION,          // 0x22
        &&RESERVED_INSTRUCTION,          // 0x23
        &&RESERVED_INSTRUCTION,          // 0x24
        &&RESERVED_INSTRUCTION,          // 0x25
        &&RESERVED_INSTRUCTION,          // 0x26
        &&RESERVED_INSTRUCTION,          // 0x27
        &&RESERVED_INSTRUCTION,          // 0x28
        &&RESERVED_INSTRUCTION,          // 0x29
        &&RESERVED_INSTRUCTION,          // 0x2A
        &&RESERVED_INSTRUCTION,          // 0x2B
        &&RESERVED_INSTRUCTION,          // 0x2C
        &&RESERVED_INSTRUCTION,          // 0x2D
        &&RESERVED_INSTRUCTION,          // 0x2E
        &&RESERVED_INSTRUCTION,          // 0x2F
        &&RESERVED_INSTRUCTION,          // 0x30
        &&RESERVED_INSTRUCTION,          // 0x31
        &&RESERVED_INSTRUCTION,          // 0x32
        &&RESERVED_INSTRUCTION,          // 0x33
        &&RESERVED_INSTRUCTION,          // 0x34
        &&RESERVED_INSTRUCTION,          // 0x35
        &&RESERVED_INSTRUCTION,          // 0x36
        &&RESERVED_INSTRUCTION,          // 0x37
        &&RESERVED_INSTRUCTION,          // 0x38
        &&RESERVED_INSTRUCTION,          // 0x39
        &&RESERVED_INSTRUCTION,          // 0x3A
        &&RESERVED_INSTRUCTION,          // 0x3B
        &&RESERVED_INSTRUCTION,          // 0x3C
        &&RESERVED_INSTRUCTION,          // 0x3D
        &&RESERVED_INSTRUCTION,          // 0x3E
        &&SDBBP,    // 0x3F
    };
    
    static void* special3Table[64] {
        &&EXT,      // 0x00
        &&RESERVED_INSTRUCTION,          // 0x01
        &&RESERVED_INSTRUCTION,          // 0x02
        &&RESERVED_INSTRUCTION,          // 0x03
        &&INS,      // 0x04
        &&RESERVED_INSTRUCTION,          // 0x05
        &&RESERVED_INSTRUCTION,          // 0x06
        &&RESERVED_INSTRUCTION,          // 0x07
        &&RESERVED_INSTRUCTION,          // 0x08
        &&RESERVED_INSTRUCTION,          // 0x09
        &&RESERVED_INSTRUCTION,          // 0x0A
        &&RESERVED_INSTRUCTION,          // 0x0B
        &&RESERVED_INSTRUCTION,          // 0x0C
        &&RESERVED_INSTRUCTION,          // 0x0D
        &&RESERVED_INSTRUCTION,          // 0x0E
        &&RESERVED_INSTRUCTION,          // 0x0F
        &&RESERVED_INSTRUCTION,          // 0x10
        &&RESERVED_INSTRUCTION,          // 0x11
        &&RESERVED_INSTRUCTION,          // 0x12
        &&RESERVED_INSTRUCTION,          // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&RESERVED_INSTRUCTION,          // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&RESERVED_INSTRUCTION,          // 0x1F
        &&BSHFL,    // 0x20
        &&RESERVED_INSTRUCTION,          // 0x21
        &&RESERVED_INSTRUCTION,          // 0x22
        &&RESERVED_INSTRUCTION,          // 0x23
        &&RESERVED_INSTRUCTION,          // 0x24
        &&RESERVED_INSTRUCTION,          // 0x25
        &&RESERVED_INSTRUCTION,          // 0x26
        &&RESERVED_INSTRUCTION,          // 0x27
        &&RESERVED_INSTRUCTION,          // 0x28
        &&RESERVED_INSTRUCTION,          // 0x29
        &&RESERVED_INSTRUCTION,          // 0x2A
        &&RESERVED_INSTRUCTION,          // 0x2B
        &&RESERVED_INSTRUCTION,          // 0x2C
        &&RESERVED_INSTRUCTION,          // 0x2D
        &&RESERVED_INSTRUCTION,          // 0x2E
        &&RESERVED_INSTRUCTION,          // 0x2F
        &&RESERVED_INSTRUCTION,          // 0x30
        &&RESERVED_INSTRUCTION,          // 0x31
        &&RESERVED_INSTRUCTION,          // 0x32
        &&RESERVED_INSTRUCTION,          // 0x33
        &&RESERVED_INSTRUCTION,          // 0x34
        &&RESERVED_INSTRUCTION,          // 0x35
        &&RESERVED_INSTRUCTION,          // 0x36
        &&RESERVED_INSTRUCTION,          // 0x37
        &&RESERVED_INSTRUCTION,          // 0x38
        &&RESERVED_INSTRUCTION,          // 0x39
        &&RESERVED_INSTRUCTION,          // 0x3A
        &&RESERVED_INSTRUCTION,          // 0x3B
        &&RESERVED_INSTRUCTION,          // 0x3C
        &&RESERVED_INSTRUCTION,          // 0x3D
        &&RESERVED_INSTRUCTION,          // 0x3E
        &&RESERVED_INSTRUCTION,          // 0x3F
    };
    
    static void* regimmTable[32] {
        &&BLTZ,     // 0x00
        &&BGEZ,     // 0x01
        &&BLTZL,    // 0x02
        &&BGEZL,    // 0x03
        &&RESERVED_INSTRUCTION,          // 0x04
        &&RESERVED_INSTRUCTION,          // 0x05
        &&RESERVED_INSTRUCTION,          // 0x06
        &&RESERVED_INSTRUCTION,          // 0x07
        &&TGEI,     // 0x08
        &&TGEIU,    // 0x09
        &&TLTI,     // 0x0A
        &&TLTIU,    // 0x0B
        &&TEQI,     // 0x0C
        &&RESERVED_INSTRUCTION,          // 0x0D
        &&TNEI,     // 0x0E
        &&RESERVED_INSTRUCTION,          // 0x0F
        &&BLTZAL,   // 0x10
        &&BGEZAL,   // 0x11
        &&BLTZALL,  // 0x12
        &&BGEZALL,  // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&RESERVED_INSTRUCTION,          // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&RESERVED_INSTRUCTION,          // 0x1F
    };
    
    static void* cop0Table[32] {
        &&MFC0,     // 0x00
        &&RESERVED_INSTRUCTION,          // 0x01
        &&RESERVED_INSTRUCTION,          // 0x02
        &&RESERVED_INSTRUCTION,          // 0x03
        &&MTC0,     // 0x04
        &&RESERVED_INSTRUCTION,          // 0x05
        &&RESERVED_INSTRUCTION,          // 0x06
        &&RESERVED_INSTRUCTION,          // 0x07
        &&RESERVED_INSTRUCTION,          // 0x08
        &&RESERVED_INSTRUCTION,          // 0x09
        &&RESERVED_INSTRUCTION,          // 0x0A
        &&MFMC0,       // 0x0B
        &&RESERVED_INSTRUCTION,          // 0x0C
        &&RESERVED_INSTRUCTION,          // 0x0D
        &&RESERVED_INSTRUCTION,          // 0x0E
        &&RESERVED_INSTRUCTION,          // 0x0F
        &&RESERVED_INSTRUCTION,          // 0x10
        &&RESERVED_INSTRUCTION,          // 0x11
        &&RESERVED_INSTRUCTION,          // 0x12
        &&RESERVED_INSTRUCTION,          // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&RESERVED_INSTRUCTION,          // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&RESERVED_INSTRUCTION,          // 0x1F
    };
    
    static void* cop0COTable[32] {
        &&RESERVED_INSTRUCTION,          // 0x00
        &&TLBR,     // 0x01
        &&TLBWI,    // 0x02
        &&RESERVED_INSTRUCTION,          // 0x03
        &&RESERVED_INSTRUCTION,          // 0x04
        &&RESERVED_INSTRUCTION,          // 0x05
        &&TLBWR,    // 0x06
        &&RESERVED_INSTRUCTION,          // 0x07
        &&TLBP,     // 0x08
        &&RESERVED_INSTRUCTION,          // 0x09
        &&RESERVED_INSTRUCTION,          // 0x0A
        &&RESERVED_INSTRUCTION,          // 0x0B
        &&RESERVED_INSTRUCTION,          // 0x0C
        &&RESERVED_INSTRUCTION,          // 0x0D
        &&RESERVED_INSTRUCTION,          // 0x0E
        &&RESERVED_INSTRUCTION,          // 0x0F
        &&RESERVED_INSTRUCTION,          // 0x10
        &&RESERVED_INSTRUCTION,          // 0x11
        &&RESERVED_INSTRUCTION,          // 0x12
        &&RESERVED_INSTRUCTION,          // 0x13
        &&RESERVED_INSTRUCTION,          // 0x14
        &&RESERVED_INSTRUCTION,          // 0x15
        &&RESERVED_INSTRUCTION,          // 0x16
        &&RESERVED_INSTRUCTION,          // 0x17
        &&ERET,     // 0x18
        &&RESERVED_INSTRUCTION,          // 0x19
        &&RESERVED_INSTRUCTION,          // 0x1A
        &&RESERVED_INSTRUCTION,          // 0x1B
        &&RESERVED_INSTRUCTION,          // 0x1C
        &&RESERVED_INSTRUCTION,          // 0x1D
        &&RESERVED_INSTRUCTION,          // 0x1E
        &&DERET,    // 0x1F
    };
    
    // Begin Dispatch Loop
    // Start Count/Compare timer
    cop0.startCounter(this);
    
dispatchStart:
    try {
        exceptRestartLoop = false;
        
    #ifdef TEST_PROJECT
        updateISARep() checkForInts() fetch() DECODE_OPCODE(); goto *opcodeTable[opcode];
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
            throw IntegerOverflowException();
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
        // Check if allowed
        if (!cop0.inKernelMode()) {
            if ((cop0.getRegister(CO0_STATUS) & STATUS_CU0) == 0) {
                throw CoprocessorUnusableException(CoprocessorUnusableException::FaultingCoprocessor::CO0);
            }
        }
        DECODE_CO();
        if (co == 0) {
            DECODE_RS();
            goto *cop0Table[rs];
        }
        else {
            DECODE_FUNCT();
            goto *cop0COTable[funct];
        }
    
    // 0x11 COP1
    COP1:
        // FIXME: I _think_ this is reserved, unusable only happens if the coprocessor exists
        throw ReservedInstructionException();
    
    // 0x12 COP2
    COP2:
        throw ReservedInstructionException();
        
    // 0x13
    COP1X:
        throw ReservedInstructionException();
    
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        memory->readByte(tempi32, &tempu8, CPUNUM, &cop0);
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
        memory->readHalf(tempi32, &tempu16, CPUNUM, &cop0);
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
        memory->readHalfUnaligned(tempi32, tempi32+1, &tempu16, CPUNUM, &cop0);
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
        memory->readWord(tempi32, &tempu32, CPUNUM, &cop0);
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
        memory->readByte(tempi32, &tempu8, CPUNUM, &cop0);
        registers[rt] = tempu8;
        DISPATCH();
    
    // 0x25 Load Halfword Unsigned
    LHU:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readHalf(tempi32, &tempu16, CPUNUM, &cop0);
        registers[rt] = tempu16;
        DISPATCH();
    
    // 0x26 Load Word Right
    LWR:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->readHalfUnaligned(tempi32-1, tempi32, &tempu16, CPUNUM, &cop0);
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
        memory->storeByte(tempi32, registers[rt], CPUNUM, &cop0);
        DISPATCH();
    
    // 0x29 Store Halfword
    SH:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->storeHalf(tempi32, registers[rt], CPUNUM, &cop0);
        DISPATCH();
    
    // 0x2A Store Word Left
    SWL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->storeHalfUnaligned(tempi32, tempi32+1, (registers[rt] & 0xFFFF0000) >> 16, CPUNUM, &cop0);
        DISPATCH();
    
    // 0x2B Store Word
    SW:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->storeWord(tempi32, registers[rt], CPUNUM, &cop0);
        DISPATCH();
    
    // 0x2C
    
    // 0x2D
    
    // 0x2E Store Word Right
    SWR:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = registers[rs] + immse;
        memory->storeHalfUnaligned(tempi32-1, tempi32, registers[rt] & 0x0000FFFF, CPUNUM, &cop0);
        DISPATCH();
    
    // 0x2F Cache
    CACHE:
        //goto UNIMPLEMENTED_INSTRUCTION;
        DISPATCH();
    
    // 0x30 Load Linked Word
    LL:
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        memory->readWordLL(tempi32, &tempu32, CPUNUM, &cop0);
        // Start RWM sequence
        LLPFN = (tempi32&0xFFFFF000) >> 12;
        LLBit = true;
        tempi32 = tempu32;
        registers[rt] = tempi32;
        DISPATCH();
    
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
        DECODE_RS();
        DECODE_RT();
        DECODE_IMMSE();
        tempi32 = immse;
        tempi32 += registers[rs];
        // Check if this is the same PFN.
        // FIXME: No this does not truely work across address translations.
        // E.x. You could LL on one address and SC on another address if they're
        // in the same frame.
        // MIPS does not specify whether that's valid or not at all.
        if ((LLBit) && (LLPFN == ((tempi32&0xFFFFF000) >> 12))) {
            registers[rt] = memory->storeWordSC(tempi32, registers[rt], CPUNUM, &cop0) == true ? 1 : 0;
            // If successful then end RWM sequence
            LLBit = registers[rt] > 0 ? false : true;
        }
        else {
            registers[rt] = false;
        }
        DISPATCH();
    
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
        // FIXME: checks Config1.CA, MIPS16e not implemented
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
        throw SystemCallException();
    
    // 0x0D Breakpoint
    BREAK:
        throw BreakpointException();
    
    // 0x0E
    
    // 0x0F Sync
    SYNC:
        //goto UNIMPLEMENTED_INSTRUCTION;
        DISPATCH();
    
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
            throw IntegerOverflowException();
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
            throw IntegerOverflowException();
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
        DECODE_RS();
        DECODE_RT();
        if ((int32_t)registers[rs] >= (int32_t)registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x31 Trap if Greater or Equal Unsigned
    TGEU:
        DECODE_RS();
        DECODE_RT();
        if (registers[rs] >= registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x32 Trap if Less Than
    TLT:
        DECODE_RS();
        DECODE_RT();
        if ((int32_t)registers[rs] < (int32_t)registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x33 Trap if Less Than Unsigned
    TLTU:
        DECODE_RS();
        DECODE_RT();
        if (registers[rs] < registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x34 Trap if Equal
    TEQ:
        DECODE_RS();
        DECODE_RT();
        if ((int32_t)registers[rs] == (int32_t)registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x35 Trap if Not Equal
    TNE:
        DECODE_RS();
        DECODE_RT();
        if ((int32_t)registers[rs] != (int32_t)registers[rt]) {
            throw TrapException();
        }
        DISPATCH();
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
                goto RESERVED_INSTRUCTION;
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        else {
            PC += 4;    // Skip delay slot instruction
        }
        DISPATCH();
    
    // 0x08 Trap if Greater or Equal Immediate
    TGEI:
        DECODE_RS();
        DECODE_IMMSE();
        if ((int32_t)registers[rs] >= immse) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x09 Trap if Greater or Equal Immediate Unsigned
    TGEIU:
        DECODE_RS();
        DECODE_IMMSE();
        if (registers[rs] >= (uint32_t)immse) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x0A Trap if Less Than Immediate
    TLTI:
        DECODE_RS();
        DECODE_IMMSE();
        if ((int32_t)registers[rs] < immse) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x0B Trap if Less Than Immediate Unsigned
    TLTIU:
        DECODE_RS();
        DECODE_IMMSE();
        if (registers[rs] < (uint32_t)immse) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x0C Trap if Equal Immediate
    TEQI:
        DECODE_RS();
        DECODE_IMMSE();
        if ((int32_t)registers[rs] == immse) {
            throw TrapException();
        }
        DISPATCH();
    
    // 0x0E Trap if Not Equal
    TNEI:
        DECODE_RS();
        DECODE_IMMSE();
        if ((int32_t)registers[rs] != immse) {
            throw TrapException();
        }
    
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        else {
            PC += 4;    // Skip delay slot instruction
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
        registers[rt] = cop0.getRegisterSW(rd, sel);
        DISPATCH();
    
    // 0x04 Move To Coprocessor0
    MTC0:
        DECODE_SEL();
        DECODE_RT();
        DECODE_RD();
        cop0.setRegisterSW(rd, sel, registers[rt]);
        DISPATCH();
    
    // 0x0B
    MFMC0:
        DECODE_SC();
        if (sc == 0) {
            // DI: Disable Interrupts
            DECODE_RT();
            registers[rt] = cop0.getRegister(CO0_STATUS);
            cop0.andRegisterHW(CO0_STATUS, ~STATUS_IE);
        }
        else {
            // EI: Enable Interrupts
            DECODE_RT();
            registers[rt] = cop0.getRegister(CO0_STATUS);
            cop0.orRegisterHW(CO0_STATUS, STATUS_IE);
        }
        DISPATCH();
/*
 * === END COP0 ===
 */
        
/*
 *  === COP0 CO Instructions ===
 */
    // 0x01 Read Indexed TLB Entry
    TLBR:
        memory->readTLB(CPUNUM, &cop0);
        DISPATCH();
        
    // 0x02 Write Indexed TLB Entry
    TLBWI:
        memory->writeIndexedTLB(CPUNUM, &cop0);
        DISPATCH();
        
    // 0x06 Write Random TLB Entry
    TLBWR:
        memory->writeRandomTLB(CPUNUM, &cop0);
        DISPATCH();
        
    // 0x08 Probe TLB for Matching Entry
    TLBP:
        memory->probeTLB(CPUNUM, &cop0);
        DISPATCH();
        
    // 0x18 Exception Return
    ERET:
        tempu32 = 0;
        // If Status_erl = 1
        if ((cop0.getRegister(CO0_STATUS) & STATUS_ERL) > 0) {
            tempu32 = cop0.getRegister(CO0_ERROREPC);
            // Status_erl = 0
            cop0.andRegisterHW(CO0_STATUS, ~(STATUS_ERL));
        }
        else {
            tempu32 = cop0.getRegister(CO0_EPC);
            // Status_exl = 0
            cop0.andRegisterHW(CO0_STATUS, ~(STATUS_EXL));
            // FIXME: Shadow Registers not implemented (optional)
        }
        // TODO: MIPS16e not implemented (optional)
        PC = tempu32;
        LLBit = false;
        
        // FIXME: Manual is extremely unclear about how and when CAUSE_IP bits are cleared
        // If they aren't we're just going to interrupt loop forever
        clearInterrupt(lastReceivedInt);
        
        DISPATCH();
    
    // 0x1F Debug Exception Return
    DERET:
        goto RESERVED_INSTRUCTION;
        //DISPATCH();
        
/*
 * === END COP0 CO ===
 */

/*
 * === Miscellaneous ===
 */
    // Handles thread signal reception
    // This will dispatch to specific
    // signal handlers
    HANDLE_SIGNAL:
        switch (signal) {
            case BREAKPTACTIVE: {
                goto BREAKPTACTIVE;
            }
            case PAUSE: {
                goto PAUSE;
            }
            case HALT: {
                goto CPU_HALT;
            }
            case STEP: {
                goto STEP;
            }
            case CONTINUE: {
            default:
                goto CONTINUE;
            }
        }
    
    // Handles single stepping
    // Basically continue once and pause
    // on next fetch.
    STEP:
        signal = PAUSE;
        goto CONTINUE;
        
    // Handles breakpointing
    // Just polls the current PC
    // otherwise continues.
    BREAKPTACTIVE:
        if (PC == breakpointTarget) {
            signal = PAUSE;
            goto PAUSE;
        }
        else {
            goto CONTINUE;
        }
        
    // Waits for external thread signaling
    PAUSE:
        // Wait for signal to change
        while (signal == PAUSE) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        goto HANDLE_SIGNAL;
        
    // Resumes execution after handling a thread signal event
    CONTINUE:
        // Continue execution
        checkForInts() fetch() DECODE_OPCODE(); goto *opcodeTable[opcode];
        
    // Jumped to from dispatch() upon the detection of an interrupt request
    HANDLE_INTERRUPT:
        if ((cop0.getRegister(CO0_STATUS) & STATUS_INTS) == 0x1) {
            serviceInterrupt();
        }
        // If we reached here then an interrupt was not taken so continue
        // the fetch, decode and execution
        fetch() DECODE_OPCODE(); goto *opcodeTable[opcode];
    // End HANDLE_INTERRUPT
    
    // Triggered when a reserved instruction is decoded
    RESERVED_INSTRUCTION:
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg("Reserved Instruction!");
            consoleUI->sendConsoleMsg(debugPrint());
        }
        throw ReservedInstructionException();
    
    // Triggered when an unimplemented opcode is decoded
    UNIMPLEMENTED_INSTRUCTION:
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg("ERROR: Unimplemented instruction!");
            consoleUI->sendConsoleMsg(debugPrint());
        }
        // This is a good point to pause and break into the debugger
        signal = PAUSE;
        goto HANDLE_SIGNAL;
    
    // More or less the only way out of dispatchLoop()
    CPU_HALT:
        if (consoleUI != nullptr) {
            std::string msg = std::to_string(cycleCounter);
            msg += " CPU Halted.";
            consoleUI->sendConsoleMsg(msg);
        }
        return;
        
    #ifdef TEST_PROJECT
    // Throws an exception based on jump immediate value
    TRIGGER_EXCEPTION:
        executeException();
    #endif
        
/*
 * === END Miscellaneous ===
 */
    }
    // Multi-catch for in processor interrupts
    // exceptions and simulation errors
    catch (std::runtime_error& e) {
        exceptRestartLoop = false;
        
        if (consoleUI != nullptr) {
            consoleUI->sendConsoleMsg(e.what());
            consoleUI->sendConsoleMsg(debugPrint());
        }
    }
    // MIPS Exceptions
    catch (MIPSException& e) {
        // Execute exception handler
        e.execute(this);
        
        // Turn off branch controls
        branchDelay = false;
        branch = false;

    #ifdef TEST_PROJECT
        exceptRestartLoop = false;
    #else
        exceptRestartLoop = true;
    #endif
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

// CPU Thread interrupt servicing
// Tests based on priority so the highest will interrupt first
void CPU::serviceInterrupt() {
    uint32_t cause = cop0.getRegister(CO0_CAUSE);
    uint32_t status = cop0.getRegister(CO0_STATUS);
    // HW5
    if (((cause & CAUSE_IP7) > 0) && ((status & STATUS_IM7) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW5;
        throw InterruptException();
    }
    // HW4
    else if (((cause & CAUSE_IP6) > 0) && ((status & STATUS_IM6) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW4;
        throw InterruptException();
    }
    // HW3
    else if (((cause & CAUSE_IP5) > 0) && ((status & STATUS_IM5) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW3;
        throw InterruptException();
    }
    // HW2
    else if (((cause & CAUSE_IP4) > 0) && ((status & STATUS_IM4) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW2;
        throw InterruptException();
    }
    // HW1
    else if (((cause & CAUSE_IP3) > 0) && ((status & STATUS_IM3) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW1;
        throw InterruptException();
    }
    // HW0
    else if (((cause & CAUSE_IP2) > 0) && ((status & STATUS_IM2) > 0)) {
        lastReceivedInt = MIPSInterrupt::HW0;
        throw InterruptException();
    }
    // SW1
    else if (((cause & CAUSE_IP1) > 0) && ((status & STATUS_IM1) > 0)) {
        lastReceivedInt = MIPSInterrupt::SW1;
        throw InterruptException();
    }
    // SW0
    else if (((cause & CAUSE_IP0) > 0) && ((status & STATUS_IM0) > 0)) {
        lastReceivedInt = MIPSInterrupt::SW0;
        throw InterruptException();
    }
}

// Sets a breakpoint target
void CPU::setBreakpoint(uint32_t addr) {
    breakpointTarget = addr;
}

// Sends a signal to the CPU thread
void CPU::sendThreadSignal(CPUSignal sig) {
    signal = sig;
}

// Gets the current thread state
CPU::CPUSignal CPU::getThreadSignal() {
    return this->signal;
}