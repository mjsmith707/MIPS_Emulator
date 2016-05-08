//
//  COP0Register.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "COP0Register.h"

/*
 * This is a copy and paste job from the c# version until I can figure out
 * a better (and faster) way to reimplement all of this
 */

// Default Constructor
COP0Register::COP0Register() : resetValue(0) {
    for (size_t i=0; i<32; i++) {
        bitfields[i] = READWRITE;
    }
};

// Parameterized Constructor
COP0Register::COP0Register(uint32_t value, uint32_t mask1, uint32_t mask2) : resetValue(value) {
    this->copregister = value;
    setRWMask(mask1, mask2);
}

// Sets Register Bit Read/Write locks
// E.x.
// Bit:	  3210
// Mask1: 0111
// Mask2: 0011
// LOCK bit3, READ bit 2, READWRITE bit 1, READWRITE bit 0
void COP0Register::setRWMask(uint32_t mask1, uint32_t mask2) {
    unsigned char value1 = 0;
    unsigned char value2 = 0;
    const uint32_t bitmask = 0x1;
    
    for (int i=0; i<32; i++) {
        value1 = (unsigned char)((mask1&(bitmask << i)) >> i);
        value2 = (unsigned char)((mask2&(bitmask << i)) >> i);
        if ((value1 == 0) && (value2 == 0)) {
            bitfields[i] = LOCKED;
        }
        else if ((value1 == 1) && (value2 == 0)) {
            bitfields[i] = READ;
        }
        else if ((value1 == 1) && (value2 == 1)) {
            bitfields[i] = READWRITE;
        }
        else {
            throw new std::runtime_error("Invalid RWX Mask for CPC0 register.");
        }
    }
}

// Returns the value in copregister
uint32_t COP0Register::getValue() {
    return this->copregister;
}

#ifdef TEST_PROJECT
// For unit testing
uint32_t COP0Register::getResetValue() {
    return this->resetValue;
}
#endif

// Need to honor the RW Mask array.
// Hardware can Write to READ bits but not LOCKED
// Software can Write to READWRITE bits but not LOCKED or READ
// Writing to LOCKED field is UNDEFINED by architecture
// but we'll just ignore it too.
void COP0Register::updateValue(uint32_t value, bool hwmode) {
    uint32_t bit1 = 0;
    uint32_t bit2 = 0;
    uint32_t newvalue = 0;
    const uint32_t mask = 0x01;
    // Read each bit and compare to their bitfield mask value
    // If permissible. Modify the newvalue.
    // bit1 is the current bit in the register
    // bit2 is the current bit in the parameter
    for (int i = 0; i < 32; i++) {
        bit1 = (this->copregister & (mask << i));
        bit2 = (value & (mask << i));
        if (bitfields[i] == LOCKED) {
            // Ignore
            newvalue |= bit1;
        }
        else if (bitfields[i] == READ) {
            if (hwmode) {
                // Hardware Writable
                newvalue |= bit2;
            }
            else {
                // Ignore
                newvalue |= bit1;
            }
        }
        else if (bitfields[i] == READWRITE) {
            // Write Always
            newvalue |= bit2;
        }
        else {
            throw new std::runtime_error("Invalid RWX Mask for CPC0 register during writing.");
        }
    }
    
    // Update Register Value
    this->copregister = newvalue;
}

// Mainly for quickly incrementing counter registers like Random
// which should only be accessed by the cpu thread anyway
void COP0Register::setValueUnsafe(uint32_t value, bool hwmode) {
    updateValue(value, hwmode);
}

// Sets the register to a value
void COP0Register::setValue(uint32_t value, bool hwmode) {
    std::lock_guard<std::mutex> lockg(mute);
    updateValue(value, hwmode);
}

// Does an atomic and with the register and value
void COP0Register::andValue(uint32_t value, bool hwmode) {
    std::lock_guard<std::mutex> lockg(mute);
    updateValue(getValue() & value, hwmode);
}

// Does an atomic or with the register and value
void COP0Register::orValue(uint32_t value, bool hwmode) {
    std::lock_guard<std::mutex> lockg(mute);
    updateValue(getValue() | value, hwmode);
}

// Does an atomic add
void COP0Register::addValue(uint32_t value, bool hwmode) {
    std::lock_guard<std::mutex> lockg(mute);
    updateValue(getValue() + value, hwmode);
}

// Resets register to its original value
void COP0Register::resetRegister() {
    std::lock_guard<std::mutex> lockg(mute);
    this->copregister = this->resetValue;
}