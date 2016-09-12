//
//  MC146818.cpp
//  MIPS_Emulator
//
//  Created by Matt on 5/12/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "MC146818.h"
// Constructor
MC146818::MC146818() {
    // CMOS_RTC Addresses
    //addresses.push_back(CMOS_BASE);
    //addresses.push_back(CMOS_BASE+63);
    addresses.push_back(CMOS_BASEPHYS);
    addresses.push_back(CMOS_BASEPHYS+63);
    cpu = nullptr;
    cycleInterval = CLOCKSPEEDHZ;;
}

// Destructor
MC146818::~MC146818() {
    
}

// Called by MMU to initialize the device
void MC146818::initDevice() {
    for (size_t i=0; i<64; i++) {
        registers[i] = 0;
    }
    registers[0x1] = 0x1;
}

// Updates clock registers
void MC146818::updateClock() {
    time(&utctime);
    ptm = gmtime(&utctime);
    
    // Seconds
    registers[0] = ptm->tm_sec;
    
    // Minutes
    registers[2] = ptm->tm_min;
    
    // Hours
    registers[4] = ptm->tm_hour;
    
    // Day of Week
    registers[6] = ptm->tm_wday;
    
    // Day of Month
    registers[7] = ptm->tm_mday;
    
    // Month
    registers[8] = ptm->tm_mon;
    
    // Year
    registers[9] = ptm->tm_year;
}

// Called by MMU to get address mappings
std::vector<uint32_t> MC146818::getAddresses() {
    return this->addresses;
}

// Called by MMU to read a byte
uint8_t MC146818::readByte(uint32_t address) {
    //address &= 0x3F;
    address -= CMOS_BASEPHYS;

    if (address < 64) {
        return registers[address];
    }
    else {
        std::string msg = "Got invalid address in MC146818::readByte: ";
        msg += std::to_string(address);
        throw std::runtime_error(msg);
    }

}

// Called by MMU to store a byte
void MC146818::storeByte(uint32_t address, uint8_t value) {
    //address &= 0x3F;
    address -= CMOS_BASEPHYS;

    switch (address) {
        // Seconds
        case 0: {
            registers[address] = value;
            break;
        }
        // Seconds Alarm
        case 1: {
            registers[address] = value;
            break;
        }
        // Minutes
        case 2: {
            registers[address] = value;
            break;
        }
        // Minutes Alarm
        case 3: {
            registers[address] = value;
            break;
        }
        // Hours
        case 4: {
            registers[address] = value;
            break;
        }
        // Hours Alarm
        case 5: {
            registers[address] = value;
            break;
        }
        // Day of Week
        case 6: {
            registers[address] = value;
            break;
        }
        // Day of Month
        case 7: {
            registers[address] = value;
            break;
        }
        // Month
        case 8: {
            registers[address] = value;
            break;
        }
        // Year
        case 9: {
            registers[address] = value;
            break;
        }
        // Register A
        // UIP bit 8 is read only
        case 10: {
            registers[address] = (registers[address] & 0x80) | (value & 0x7F);
            break;
        }
        // Register B
        case 11: {
            registers[address] = value;
            break;
        }
        // Register C
        case 12: {
            registers[address] = value;
            break;
        }
        // Register D
        case 13: {
            registers[address] = value;
            break;
        }
        default: {
            if ((address > 13) && (address < 64)) {
                registers[address] = value;
            }
            else {
                std::string msg = "Got invalid address in MC146818::storeByte: ";
                msg += std::to_string(address);
                throw std::runtime_error(msg);
            }
        }
    }
}

// Used to get the interval between executions
uint32_t MC146818::getCycleInterval() {
    return cycleInterval;
}

// Indicates to the CPU that the interval is enabled
bool MC146818::isEnabled() {
    return true;
}

// Called by CPU when cycle interval is reached
void MC146818::onCycleExecute() {
    // Check if we are in a time update period
    if ((registers[0x1] & 0x80u) == 0) {
        // Set UIP to 1 in Register A
        registers[0x1] |= 0x80u;
        // Set interval to match 248 milliseconds
        cycleInterval = 1000*248*CLOCKSPEEDMHZ;
    }
    else {
        // Update clock
        updateClock();
        // Set UIP to 0 in Register A
        registers[0x1] &= ~(0x80u);
        // Set interval to match 1 second
        cycleInterval = CLOCKSPEEDHZ;
    }
}

// Gets a pointer to the CPU so it can call sendInterrupt etc.
void MC146818::getCPUHandle(CPU* vcpu) {
    if (vcpu == nullptr) {
        throw std::runtime_error("Got nullptr in MC146818::getCPUHandle");
    }
    this->cpu = vcpu;
}