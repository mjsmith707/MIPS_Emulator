//
//  UART8250.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "UART8250.h"

// Default Constructor
UART8250::UART8250() {
    // UART Addresses
    addresses.push_back(UART_BASE);
    addresses.push_back(UART_BASE+7);
    addresses.push_back(UART_BASEPHYS);
    addresses.push_back(UART_BASEPHYS+7);
};

// Device initializer
void UART8250::initDevice() {
    THR = 0x0;
    RBR = 0x0;
    DLL = 0x0;
    IER = 0x0;
    DLH = 0x0;
    IIR = 0x0;
    FCR = 0x0;
    LCR = 0x0;
    MCR = 0x0;
    LSR = 0x60;
    MSR = 0x0;
    SR = 0x0;
}

std::vector<uint32_t> UART8250::getAddresses() {
    return addresses;
}

inline bool UART8250::DLABSet() {
    return (LCR & 0x40) > 0;
}

uint8_t UART8250::readByte(uint32_t address) {
    address &= 0xF;
    address -= addrOffset;
    switch (address) {
        case 0: {
            if (DLABSet()) {
                return DLL;
            }
            else {
                if (!sendBuffer.isEmpty()) {
                    RBR = sendBuffer.pop();
                }
                // Clear Data Ready bit
                if (sendBuffer.isEmpty()) {
                    LSR &= ~0x1;
                }
                
                return RBR;
            }
        }
        case 1: {
            if (DLABSet()) {
                return DLH;
            }
            else {
                return IER;
            }
        }
        case 2: {
            return IIR;
        }
        case 3: {
            return LCR;
        }
        case 4: {
            return MCR;
        }
        case 5: {
            return LSR;
        }
        case 6: {
            return MSR;
        }
        case 7: {
            return SR;
        }
        default: {
            throw std::runtime_error("Memory tried to read from invalid UART address");
        }
    }
}

void UART8250::storeByte(uint32_t address, uint8_t value) {
    address &= 0xF;
    address -= addrOffset;
    switch (address) {
        case 0: {
            if (DLABSet()) {
                DLL = value;
            }
            else {
                THR = value;
                receiveBuffer.push(THR);
                //LSR &= ~0x40;
            }
            break;
        }
        case 1: {
            if (DLABSet()) {
                DLH = value;
            }
            else {
                IER = value;
            }
            break;
        }
        case 2: {
            FCR = value;
            break;
        }
        case 3: {
            LCR = value;
            break;
        }
        case 4: {
            MCR = value;
            break;
        }
        case 5: {
            break;
        }
        case 6: {
            break;
        }
        case 7: {
            SR = value;
            break;
        }
        default: {
            throw std::runtime_error("Memory tried to write to invalid UART address");
        }
    }
}
// 01000000
// 01100000
// Method to receive character from console
void UART8250::sendChar(char ch) {
    // Save to buffer
    sendBuffer.push(ch);
    // Signal that there is data available
    LSR |= 0x1;
}

// Method to send a char to stdout
bool UART8250::getChar(char* ch) {
    if (!receiveBuffer.isEmpty()) {
        *ch = receiveBuffer.pop();
        LSR |= 0x40;
        return true;
    }
    else {
        return false;
    }
    /*
    if ((LSR & 0x40) == 0) {
        *ch = THR;
        LSR |= 0x40;
        return true;
    }
    else {
        return false;
    }
     */
}