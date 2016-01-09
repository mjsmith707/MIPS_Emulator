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

class Coprocessor0 {
    private:
        COP0Register* registerFile[32][32];
    public:
        Coprocessor0();
        uint32_t getRegister(uint8_t regnum, uint8_t sel);
        void setRegister(uint8_t regnum, uint8_t sel, uint32_t value);
        void setRegisterHW(uint8_t regnum, uint8_t sel, uint32_t value);
};

#endif /* Coprocessor0_h */
