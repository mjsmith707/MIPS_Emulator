//
//  PMMU.cpp
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
        frameTable[i] = nullptr;
        frameBoolTable[i] = false;
    }
    frameTableSize = 0;
    mmioAddressTableSize = 0;
    // Init TLB
    for (size_t i=0; i<MAXCPUS; i++) {
        for (size_t j=0; j<TLBMAXENTRIES; j++) {
            TLBTable[i][j].V1 = false;
            TLBTable[i][j].V0 = false;
        }
    }
}

// Destructor
// Cleans up frame table
PMMU::~PMMU() {
    // Clean up frame table
    for (size_t i=0; i<frameTableMax; i++) {
        if (frameTable[i] != nullptr) {
            delete[] frameTable[i];
            frameTable[i] = nullptr;
        }
        frameBoolTable[i] = false;
    }
    // Invalidate all TLB entries
    for (size_t i=0; i<MAXCPUS; i++) {
        for (size_t j=0; j<TLBMAXENTRIES; j++) {
            TLBTable[i][j].V1 = false;
            TLBTable[i][j].V0 = false;
        }
    }
}

// Static members
size_t PMMU::frameTableSize;
size_t PMMU::frameTableRamLimit;
uint8_t* PMMU::frameTable[frameTableMax];
bool PMMU::frameBoolTable[frameTableMax];
uint32_t PMMU::mmioAddressTable[MMIOADDRESSTABLEMAX];
uint32_t PMMU::mmioAddressTableSize;
MMIO_Device* PMMU::mmioDeviceTable[MMIOADDRESSTABLEMAX];
PMMU::TLBEntry_t PMMU::TLBTable[MAXCPUS][TLBMAXENTRIES];