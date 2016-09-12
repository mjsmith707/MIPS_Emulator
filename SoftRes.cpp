//
//  SoftRes.cpp
//  MIPS_Emulator
//
//  Created by Matt on 7/10/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "SoftRes.h"

SoftRes::SoftRes() {
    addresses.push_back(SOFTRES_BASE);
    addresses.push_back(SOFTRES_BASE+11);
    addresses.push_back(SOFTRES_PHYBASE);
    addresses.push_back(SOFTRES_PHYBASE+11);
}

SoftRes::~SoftRes() {
    
}

void SoftRes::initDevice() {
    registers[0] = 0x0;
    registers[1] = 0x0;
    registers[2] = 0x0;
    registers[3] = 0x0;
    registers[8] = 0x0A;
}

std::vector<uint32_t> SoftRes::getAddresses() {
    return this->addresses;
}

uint8_t SoftRes::readByte(uint32_t address) {
    address &= 0xFF;
    if (address > 11) {
        throw std::runtime_error("Memory tried to read from invalid SoftRes address");
    }
    else {
        return registers[address];
    }
}

void SoftRes::storeByte(uint32_t address, uint8_t value) {
    address &= 0xFF;
    if (address > 11) {
        throw std::runtime_error("Memory tried to read from invalid SoftRes address");
    }
    else {
        registers[address] = value;
        if (registers[3] == GORESET) {
            throw SoftResetException();
        }
    }
}