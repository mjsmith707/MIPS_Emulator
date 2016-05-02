//
//  Coprocessor0.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "Coprocessor0.h"
#include "CPU.h"

Coprocessor0::Coprocessor0() : countCompActive(false), countCompThread(nullptr) {
    // Seed RNG
    srand(time(NULL));
    
    // Initialize Registers
    // Too easy to just have normal registers
    // we have to deal with the sel field...
    // and tons of other gotchas
    // [registernum,selfield] rwmask1, rwmask2
    // MIPS Arch Vol 3
    for (unsigned int i=0; i<32; i++) {
        for (unsigned int j=0; j<32; j++) {
            registerFile[i][j] = nullptr;
        }
    }
    
    // Index Register
    // 64 Entry TLB (6 bits) so Index bits are masked as RW
    // reset: 00000000000000000000000000000000
    // mask1: 10000000000000000000000000111111
    // mask2: 00000000000000000000000000111111
    registerFile[0][0] = new COP0Register(0x0, 0x8000003F, 0x3F);
    
    // Random Register
    // Same as Index except masked R
    // FIXME: Zero TLB Entries on reset?
    // reset: 00000000000000000000000000000000
    // mask1: 10000000000000000000000000111111
    // mask2: 00000000000000000000000000000000
    registerFile[1][0] = new COP0Register(0x0, 0x8000003F, 0x0);
    
    // EntryLo0 Register, EntryLo1 Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000111111111111111111111111111
    // mask2: 00000111111111111111111111111111
    registerFile[2][0] = new COP0Register(0x0, 0x07FFFFFF, 0x07FFFFFF);
    registerFile[3][0] = new COP0Register(0x0, 0x07FFFFFF, 0x07FFFFFF);
    
    // Context Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111110000
    // mask2: 11111111100000000000000000000000
    registerFile[4][0] = new COP0Register(0x0, 0xFFFFFFF0, 0xFF800000);
    
    // ContextConfig Register
    // reset: 00000000011111111111111111110000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[4][1] = new COP0Register(0x007FFFF0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // UserLocal Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[4][2] = new COP0Register(0x0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // PageMask Register
    // reset: 00000000000000000000000000000000
    // mask1: 00011111111111111111100000000000
    // mask2: 00011111111111111111100000000000
    registerFile[5][0] = new COP0Register(0x0, 0x1FFFF800, 0x1FFFF800);
    
    // PageGrain Register
    // reset: 00000000000000000000000000000000
    // mask1: 11011000000000000000000000011111
    // mask2: 00011000000000000000000000000000
    registerFile[5][1] = new COP0Register(0x0, 0xD800001F, 0x18000000);
    
    // Wired Register
    // 64 Entries Potentially
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000111111
    // mask2: 00000000000000000000000000111111
    registerFile[6][0] = new COP0Register(0x0, 0x0000003F, 0x0000003F);
    
    // HWREna Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000011111
    // mask2: 00000000000000000000000000011111
    registerFile[7][0] = new COP0Register(0x0, 0x1F, 0x1F);
    
    // BadVAddr Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[8][0] = new COP0Register(0x0, 0xFFFFFFFF, 0x0);
    
    // Count Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[9][0] = new COP0Register(0x0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // Reserved Registers
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[9][6] = new COP0Register(0x0, 0x0, 0x0);
    registerFile[9][7] = new COP0Register(0x0, 0x0, 0x0);
    
    // EntryHi Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111100011111111
    // mask2: 11111111111111111111100011111111
    registerFile[10][0] = new COP0Register(0x0, 0xFFFFF8FF, 0xFFFFF8FF);
    
    // Compare Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[11][0] = new COP0Register(0x0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // Reserved Registers
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[11][6] = new COP0Register(0x0, 0x0, 0x0);
    registerFile[11][7] = new COP0Register(0x0, 0x0, 0x0);
    
    // Status Register
    // reset: 00000000010000000000000000000100
    // mask1: 00011110011110001111111100010111
    // mask2: 00011010011110001111111100010111
    registerFile[12][0] = new COP0Register(0x400004, 0x1E78FF17, 0x1A78FF17);
    
    // IntCtl Register
    // Timer Int = HW0
    // Counter Int = HW1
    // FIXME: Performance counters optional?
    // reset: 01001100000000000000000000000000
    // mask1: 11111111100000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[12][1] = new COP0Register(0x4C000000, 0xFF800000, 0x0);
    
    // SRSCtl Register
    // reset: 00000000000000000000000000000000
    // mask1: 00111100001111001111001111001111
    // mask2: 00000000000000001111001111000000
    registerFile[12][2] = new COP0Register(0x0, 0x3C3CF3CF, 0xF3C0);
    
    // SRSMap Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[12][3] = new COP0Register(0x0, 0x0, 0x0);
    
    // Cause Register
    // reset: 00000000000000000000000000000000
    // mask1: 10110000110000001111111101111100
    // mask2: 00000000110000000000001100000000
    registerFile[13][0] = new COP0Register(0x0, 0xB0C0FF7C, 0xC00300);
    
    // Exception Program Counter Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[14][0] = new COP0Register(0x0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // Processor Identification Register
    // FIXME: ?? No Opts, COMP_LEGACY, IMP_R3000, REV_R3000
    // old reset: 11111111000000000000001000100000
    // new reset: 00000000000000000000010000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[15][0] = new COP0Register(0x00000400, 0xFFFFFFFF, 0x0);
    
    // EBase Register
    // No write-gate
    // reset: 10000000000000000000000000000000
    // mask1: 11111111111111111111001111111111
    // mask2: 00111111111111111111000000000000
    registerFile[15][1] = new COP0Register(0x80000000, 0xFFFFF3FF, 0x3FFFF000);
    
    // Configuration Register 0
    // M = 1 (Config1 implemented)
    // K23 = 000
    // KU = 000
    // BE = 1 (Big Endian)
    // AT = 00 (MIPS32)
    // AR = 001 (Revision 2+)
    // MT = 001 (Standard TLB)
    // VI = 0 (Not Virtual ICache)
    // K0 = 010 (Uncached) Recommended 2 (Uncached) 3 (Cached) allowed
    // reset: 10000000000000001000010010000011
    // mask1: 10000000000000001111111110001111
    // mask2: 00000000000000000000000000000111
    registerFile[16][0] = new COP0Register(0x80008482, 0x8000FF8F, 0x7);
    
    // Configuration Register 1
    // M = 1 (Config2 implemented)
    // FIXME: MMU.size - 1 = 000000 ?? (Number of entries on reset)
    // IS (I-Cache sets per way) = 000 (64) FIXME: Don't care?
    // IL (I-Cache line size) = 000 (No I-Cache present) FIXME: Might need for proper OS support even if unused.
    // IA (I-Cache associativity) = 000 (Direct Mapped)
    // DS (D-Cache sets per way) = 000 (64) FIXME: Don't care?
    // DL (D-Cache line size) = 000 (No D-Cache present) FIXME: Might need for proper OS support even if unused.
    // DA (D-Cache associativity) = 000 (Direct Mapped)
    // C2 (Coprocessor 2 implemented) = 0 (No)
    // MD (MDMX ASE Implemented) = 0 (No)
    // PC (Performance Counter Registers) = 0 (No)
    // WR (Watch Registers) = 0 (No)
    // CA (MIPS16e Implemented) = 0 (No)
    // EP (EJTAG Implemented) = 0 (No)
    // FP (FPU Implemented) = 0 (No) TODO: Add FPU Support
    // Old reset: 00111111000000000000000000000000 (64 TLB)
    // reset: 10000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[16][1] = new COP0Register(0x1, 0xFFFFFFFF, 0x0);
    
    // Configuration Register 2
    // M = 1 (Config3 implemented)
    // TU = 000 (Impl-specific tertiary cache control/status, not implemented)
    // TS = 0000 (Tertiary cache sets per way Not implemented)
    // TL = 0000 (Tertiary cache line size Not implemented)
    // TA = 0000 (Tertiary cache associativity Not Implemented)
    // SU = 0000 (Secondary cache not implemented)
    // SL = 0000 (Secondary cache line size Not Implemented)
    // SA = 0000 (Secondary cache associativity Not Implemented)
    // reset: 10000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[16][2] = new COP0Register(0x1, 0xFFFFFFFF, 0x0);
    
    // Configuration Register 3
    // M = 1 (Config4 implemented)
    // CMGCR (Coherency Manager) = 0 (Not implemented)
    // MSAP (MIPS SIMD MSA) = 0 (Not implemented)
    // BP (BadInstrP register) = 0 (Not implemented)
    // BI (BadInstr register) = 0 (Not implemented)
    // SC (Segment Control registers) = 0 (Not implemented)
    // PW (Hardware Page Table Walker) = 0 (Not implemented)
    // VZ (Virtualization Module) = 0 (Cool but Not implemented)
    // IPLW (With of Status_ipl and Cause_ipl) = 00 (6 bits)
    // MMAR (microMIPS32 arch rev) = 000 (Unused see: Config3_ISA)
    // MCU (MCU ASE) = 0 (Not implemented)
    // ISAOnExec (Entry vectoring for MIPS32/microMIPS32) = 0 (microMIPS not implemented)
    // ISA (ISA availability) = 00 (Only MIPS32 Available)
    // URLI (UserLocal Register) = 0 (Not implemented)
    // RXI (RIE and XIE bits in PageGrain implemented) = 0 FIXME: In use in other parts (exception code)?
    // DSP2P (MIPS DSP Rev2) = 0 (I'd love sound too but not implemented)
    // DSPP (MIPS DSP Rev1) = 0 (See above)
    // CTXTC (ContextConfig register) = 0 (Not implemented)
    // ITL (IFlowtrace) = 0 (Not implemented)
    // LPA (Large Physical Address Support) = 0 (Not implemented)
    // VEIC (External Interrupt Controller Mode) = 0 (Not implemented)
    // VInt (Vectored Interrupts implemented) = 0 (Not implemented) FIXME: Required 1 for R2?
    // SP (1KB page support) = 0 (Not implemented) FIXME: Required 1 for R2?
    // CDMM (Common Device Memory Map) = 0 (Not implemented) TODO: This would be good to have
    // MT (MT Module) = 0 (Not implemented)
    // SM (SmartMIPS ASE) = 0 (Not implemented)
    // TL (Trace Logic) = 0 (Not implemented)
    // reset: 10000000000000000000000000000000
    // mask1: 10111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[16][3] = new COP0Register(0x80000000, 0xBFFFFFFF, 0x0);
    
    // Configuration Register 4
    // M = 0 (Config5 not implemented)
    // IE (TLB Invalidate instruction support) = 00 (Not supported)
    // AE (EntryHI_asid extended to 10 bits) = 0 (No)
    // VTLB-SizeExt = 0000
    // KScrExist (kernel-mode scratch registers available) = 00000000 (zero) FIXME: Easy to implement?
    // MMUExtDef = 00 (Reserved)
    // Bits 13-0 Reserved
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111110000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[16][4] = new COP0Register(0x0, 0xFFFFE000, 0x0);
    
    // Reserved Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[16][6] = new COP0Register(0x0, 0x0, 0x0);
    
    // Reserved Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[16][7] = new COP0Register(0x0, 0x0, 0x0);
    
    // Load Linked Address Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[17][0] = new COP0Register(0x0, 0xFFFFFFFF, 0x0);
    
    // WatchLo Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[18][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // WatchHi Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[19][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // XContext Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[20][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // Reserved Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[21][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // Reserved Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[22][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // EJTAG Debug Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[23][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // EJTAG DEPC Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[24][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // Performance Counter Register
    // Unimplemented but will be eventually
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[25][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // ErrCtl Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[26][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // CacheErr Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[27][0] = new COP0Register(0x0, 0x0, 0x0);
    registerFile[27][1] = new COP0Register(0x0, 0x0, 0x0);
    registerFile[27][2] = new COP0Register(0x0, 0x0, 0x0);
    registerFile[27][3] = new COP0Register(0x0, 0x0, 0x0);
    
    // TagLo Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[28][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // DataLo Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[28][1] = new COP0Register(0x0, 0x0, 0x0);
    
    // TagHi Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[29][0] = new COP0Register(0x0, 0x0, 0x0);
    
    // DataHi Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[29][1] = new COP0Register(0x0, 0x0, 0x0);
    
    // ErrorEPC Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 11111111111111111111111111111111
    registerFile[30][0] = new COP0Register(0x0, 0xFFFFFFFF, 0xFFFFFFFF);
    
    // DESAVE Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[31][0] = new COP0Register(0x0, 0x0, 0x0);
}

// Destructor
Coprocessor0::~Coprocessor0() {
    stopCounter();
}

// Helper functions
// Tests whether the processor is in kernel mode
bool Coprocessor0::inKernelMode() {
    return (((registerFile[12][0]->getValue() & STATUS_KSU) == 0x0)
            || ((registerFile[12][0]->getValue() & STATUS_EXL) > 0x0)
            || ((registerFile[12][0]->getValue() & STATUS_ERL) > 0x0));
}

inline bool Coprocessor0::inSupervisorMode() {
    // FIXME: Not sure if I want this 'feature' or not..
    return false;
}

// Tests whether the processor is in usermode
inline bool Coprocessor0::inUserMode() {
    return (((registerFile[12][0]->getValue() & STATUS_KSU) == 0x10)
            && ((registerFile[12][0]->getValue() & STATUS_EXL) == 0x0)
            && ((registerFile[12][0]->getValue() & STATUS_ERL) == 0x0));
}

// Tests whether interrupts are enabled
// Non-inline version to appease Clang
bool Coprocessor0::interruptsEnabled() {
    return ((registerFile[12][0]->getValue() & STATUS_INTS) == 0x1);
}

#ifdef TEST_PROJECT
// Gets the reset value for a register, for unit testing
uint32_t Coprocessor0::getRegisterReset(uint8_t regnum, uint8_t sel) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    return registerFile[regnum][sel]->getResetValue();
}
#endif

// Gets a coprocessor0 register in ISA/Software mode
uint32_t Coprocessor0::getRegisterSW(uint8_t regnum, uint8_t sel) {
    if (registerFile[regnum][sel] == nullptr) {
        // Undefined
        return 0;
    }
    // Special Behavior for Random Register
    if ((regnum == 1) && (sel == 0)) {
        // Update the random register before accessing
        updateRandom();
    }
    return registerFile[regnum][sel]->getValue();
}

// Retrives a coprocessor0 register
uint32_t Coprocessor0::getRegister(uint8_t regnum, uint8_t sel) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    return registerFile[regnum][sel]->getValue();
}

// Sets a coprocessor0 register in ISA/Software mode
void Coprocessor0::setRegisterSW(uint8_t regnum, uint8_t sel, uint32_t value) {
    if (registerFile[regnum][sel] == nullptr) {
        // Undefined
        return;
    }
    // Special behavior for Count/Compare Registers
    if ((regnum == 11) && (sel == 0)) {
        // Clear Cause_ti/IP7 (HW5)
        if ((registerFile[13][0]->getValue() & CAUSE_TI) > 0) {
            registerFile[13][0]->andValue(~CAUSE_TI, true);
            registerFile[13][0]->andValue(~CAUSE_IP7, true);
        }
    }
    // Special Behavior for Random Register
    else if ((regnum == 1) && (sel == 0)) {
        // Update the random register before accessing
        updateRandom();
    }
    registerFile[regnum][sel]->setValue(value, false);
}

// Sets a coprocessor 0 register in Hardware mode
void Coprocessor0::setRegisterHW(uint8_t regnum, uint8_t sel, uint32_t value) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->setValue(value, true);
}

// Does an atomic and on the register with mask
void Coprocessor0::andRegisterHW(uint8_t regnum, uint8_t sel, uint32_t mask) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->andValue(mask, true);
}

// Does an atomic or on the register with mask
void Coprocessor0::orRegisterHW(uint8_t regnum, uint8_t sel, uint32_t mask) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->orValue(mask, true);
}

// Resets a register to it's original reset value
void Coprocessor0::resetRegister(uint8_t regnum, uint8_t sel) {
    if (registerFile[regnum][sel] == nullptr) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->resetRegister();
}

// Increments count and tests against compare
// Triggers interrupt on count==compare
void Coprocessor0::countCompare(CPU* cpu) {
    for (;;) {
        if (!countCompActive) {
            return;
        }
        
        // Increment Count Register
        registerFile[9][0]->addValue(1, true);
        
        // Compare Count and Compare Registers
        // Only if compare != 0
        if ((registerFile[11][0]->getValue() != 0) && (registerFile[9][0]->getValue() == registerFile[11][0]->getValue())) {
            // Set Cause_ti
            registerFile[13][0]->orValue(CAUSE_TI, true);
            // Trigger HW5 interrupt
            cpu->sendInterrupt(CPU::MIPSInterrupt::HW5);
        }
        
        // FIXME: Needs to be set to some fraction of the cpu clock
        std::this_thread::sleep_for(std::chrono::nanoseconds(50));
    }
}

// Starts Count/Compare counter
void Coprocessor0::startCounter(CPU* cpu) {
    if (cpu == nullptr) {
        throw std::runtime_error("Got nullptr in cop0::startCounter");
    }
    
    // Stop counter if running
    stopCounter();
    
    // Start counter thread
    countCompActive = true;
    countCompThread = new std::thread(std::bind(&Coprocessor0::countCompare, this, cpu));
}

// Stops Count/Compare counter and thread
void Coprocessor0::stopCounter() {
    countCompActive = false;
    if (countCompThread != nullptr) {
        countCompThread->join();
        delete countCompThread;
        countCompThread = nullptr;
    }
}

// Called by CPU or Coprocessor to update Random Register
// On real hardware this is a count-down wraparound counter
// that is updated after every instruction.
// We don't need such accuracy though.
void Coprocessor0::updateRandom() {
    uint32_t wired = registerFile[6][0]->getValue();
    
    // Generate random value between wired and TLBMAXENTRIES-1
    uint32_t random = rand() % (TLBMAXENTRIES-1 - wired) + wired;
    
    registerFile[1][0]->setValue(random, true);
}