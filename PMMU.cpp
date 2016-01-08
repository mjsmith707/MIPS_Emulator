//
//  MMU.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "PMMU.h"

// Default Constructor
// Ram Size in blocks of 4096 bytes
PMMU::PMMU(size_t ramSize) {
    frameTableRamLimit = ramSize;
    for (size_t i=0; i<frameTableMax; i++) {
        frameTable[i] = NULL;
    }
    frameTableSize = 0;
}

// Destructor
// Cleans up frame table
PMMU::~PMMU() {
    for (size_t i=0; i<frameTableMax; i++) {
        if (frameTable[i] != NULL) {
            delete[] frameTable[i];
        }
    }
}

// Static members
size_t PMMU::frameTableSize;
size_t PMMU::frameTableRamLimit;
uint8_t* PMMU::frameTable[frameTableMax];