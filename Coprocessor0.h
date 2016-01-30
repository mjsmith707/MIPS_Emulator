//
//  Coprocessor0.h
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef Coprocessor0_h
#define Coprocessor0_h

#include "COP0Register.h"

// Index Register Fields (CP Register 0, Select 0)
#define INDEX_PBIT      0x80000000

// Status Register Fields (CP Register 12, Select 0)
#define STATUS_IEBIT    0x1
#define STATUS_EXLBIT   0x2
#define STATUS_ERLBIT   0x4
#define STATUS_R0BIT    0x8
#define STATUS_UMBIT    0x10
#define STATUS_KSUBITS  0x18
#define STATUS_UXBIT    0x20
#define STATUS_SXBIT    0x40
#define STATUS_KXBIT    0x80
#define STATUS_IM0BIT   0x100
#define STATUS_IM1BIT   0x200
#define STATUS_IM2BIT   0x400
#define STATUS_IM3BIT   0x800
#define STATUS_IM4BIT   0x1000
#define STATUS_IM5BIT   0x2000
#define STATUS_IM6BIT   0x4000
#define STATUS_IM7BIT   0x8000
#define STATUS_IMPL0BIT 0x10000
#define STATUS_IMPL1BIT 0x20000
#define STATUS_NMIBIT   0x80000
#define STATUS_SRBIT    0x100000
#define STATUS_TSBIT    0x200000
#define STATUS_BEVBIT   0x400000
#define STATUS_PXBIT    0x800000
#define STATUS_MXBIT    0x1000000
#define STATUS_REBIT    0x2000000
#define STATUS_FRBIT    0x4000000
#define STATUS_RPBIT    0x8000000
#define STATUS_CU0BIT   0x10000000
#define STATUS_CU1BIT   0x20000000
#define STATUS_CU2BIT   0x40000000
#define STATUS_CU3BIT   0x80000000

class Coprocessor0 {
    private:
        COP0Register* registerFile[32][32];
    public:
        Coprocessor0();
        inline bool inKernelMode();
        inline bool inSupervisorMode();
        inline bool inUserMode();
        uint32_t getRegister(uint8_t regnum, uint8_t sel);
        void setRegister(uint8_t regnum, uint8_t sel, uint32_t value);
        void setRegisterHW(uint8_t regnum, uint8_t sel, uint32_t value);
};

#endif /* Coprocessor0_h */
