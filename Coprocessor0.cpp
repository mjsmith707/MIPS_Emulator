//
//  Coprocessor0.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "Coprocessor0.h"

Coprocessor0::Coprocessor0() {
    // Initialize Registers
    // Too easy to just have normal registers
    // we have to deal with the sel field...
    // and tons of other gotchas
    // [registernum,selfield] rwmask1, rwmask2
    // MIPS Arch Vol 3
    for (unsigned int i=0; i<32; i++) {
        for (unsigned int j=0; j<32; j++) {
            registerFile[i][j] = NULL;
        }
    }
    
    // Index Register
    // 48 Entry TLB (6 bits) so Index bits are masked as RW
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
    // mask1: 11111111111111111111111111111111
    // mask2: 00111111111111111111111111111111
    registerFile[2][0] = new COP0Register(0x0, 0xFFFFFFFF, 0x3FFFFFFF);
    registerFile[3][0] = new COP0Register(0x0, 0xFFFFFFFF, 0x3FFFFFFF);
    
    // Context Register
    // reset: 00000000000000000000000000000000
    // mask1: 11111111111111111111111111110000
    // mask2: 11111111100000000000000000000000
    registerFile[4][0] = new COP0Register(0x0, 0xFFFFFFF0, 0xFF800000);
    
    // PageMask Register
    // reset: 00000000000000000000000000000000
    // mask1: 00011111111111111111000000000000
    // mask2: 00011111111111111111000000000000
    registerFile[5][0] = new COP0Register(0x0, 0x1FFFF000, 0x1FFFF000);
    
    // Wired Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000111111
    // mask2: 00000000000000000000000000111111
    registerFile[6][0] = new COP0Register(0x0, 0x0000003F, 0x0000003F);
    
    // Reserved Register
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[7][0] = new COP0Register(0x0, 0x0, 0x0);
    
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
    // mask1: 11111111111111111110000011111111
    // mask2: 11111111111111111110000011111111
    registerFile[10][0] = new COP0Register(0x0, 0xFFFFE0FF, 0xFFFFE0FF);
    
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
    // mask1: 11001110011110001111111100010111
    // mask2: 11001010011110001111111100010111
    registerFile[12][0] = new COP0Register(0x400004, 0xCE78FF17, 0xCA78FF17);
    
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
    // No Opts, COMP_LEGACY, IMP_R3000, REV_R3000
    // reset: 11111111000000000000001000100000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[15][0] = new COP0Register(0xFF000220, 0xFFFFFFFF, 0x0);
    
    // Configuration Register 0
    // reset: 10000000000000001000000010000000
    // mask1: 10000000000000001111111110000111
    // mask2: 00000000000000000000000000000111
    //								  10000011
    registerFile[16][0] = new COP0Register(0x80008080, 0x8000FF87, 0x7);
    
    // Configuration Register 1
    // 64 TLB, 64 Icache, No Icache, Direct Mapped,
    // 64 Dcache, No Dcache, Direct Mapped
    // No Coprocessor 2 (Yet)          -
    // No Perf Counter, No Watch, No Code Comp, No EJTAG
    // No FPU                                -
    // reset: 00111111000000000000000000000000
    // mask1: 11111111111111111111111111111111
    // mask2: 00000000000000000000000000000000
    registerFile[16][1] = new COP0Register(0x3F000000, 0xFFFFFFFF, 0x0);
    
    // Configuration Register 2
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[16][2] = new COP0Register(0x0, 0x0, 0x0);
    
    // Configuration Register 3
    // reset: 00000000000000000000000000000000
    // mask1: 00000000000000000000000000000000
    // mask2: 00000000000000000000000000000000
    registerFile[16][3] = new COP0Register(0x0, 0x0, 0x0);
    
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

uint32_t Coprocessor0::getRegister(uint8_t regnum, uint8_t sel) {
    if (registerFile[regnum][sel] == NULL) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    return registerFile[regnum][sel]->getValue();
}

void Coprocessor0::setRegister(uint8_t regnum, uint8_t sel, uint32_t value) {
    if (registerFile[regnum][sel] == NULL) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->setValue(value, false);
}

void Coprocessor0::setRegisterHW(uint8_t regnum, uint8_t sel, uint32_t value) {
    if (registerFile[regnum][sel] == NULL) {
        throw std::runtime_error("Invalid coprocessor register addressed!");
    }
    registerFile[regnum][sel]->setValue(value, true);
}