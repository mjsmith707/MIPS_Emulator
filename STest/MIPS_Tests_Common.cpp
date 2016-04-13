//
//  MIPS_Tests_Common.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "MIPS_Tests_Common.h"

PMMU* memory;
CPU* cpu0;

// Initializes the common components for
// testing the mips emulator
void init_tests_common() {
    memory = nullptr;
    cpu0 = nullptr;
}

// Resets the components for the mips emulator
// Should be called before every test is performed
void reset() {
    if (memory != nullptr) {
        delete memory;
    }
    if (cpu0 != nullptr) {
        delete cpu0;
    }

    memory = new PMMU(1000000);
    cpu0 = new CPU(0, memory);
}