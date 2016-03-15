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
    // Base+0
    // THR Transmitter Holding Buffer
    // RBR Receiver Buffer
    // DLL Divisor Latch Low Byte
    addresses.push_back(UART_BASE);
    
    // Base+1
    // IER Interrupt Enable Register
    // DLH Divisor Latch High Byte
    //addresses.push_back(UART_BASE+1);
    
    // Base+2
    // IIR Interrupt Identification Register
    // FCR FIFO Control Register
    //addresses.push_back(UART_BASE+2);
    
    // Base+3
    // LCR Line Control Register
    //addresses.push_back(UART_BASE+3);
    
    // Base+4
    // MCR Modem Control Register
    //addresses.push_back(UART_BASE+4);
    
    // Base+5
    // LSR Line Status Register
    //addresses.push_back(UART_BASE+5);
    
    // Base+6
    // MSR Modem Status Register
    //addresses.push_back(UART_BASE+6);
    
    // Base+7
    // SR  Scratch Register
    addresses.push_back(UART_BASE+7);
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
    switch (address) {
        case UART_BASE: {
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
        case UART_BASE+1: {
            if (DLABSet()) {
                return DLH;
            }
            else {
                return IER;
            }
        }
        case UART_BASE+2: {
            return IIR;
        }
        case UART_BASE+3: {
            return LCR;
        }
        case UART_BASE+4: {
            return MCR;
        }
        case UART_BASE+5: {
            return LSR;
        }
        case UART_BASE+6: {
            return MSR;
        }
        case UART_BASE+7: {
            return SR;
        }
        default: {
            throw std::runtime_error("Memory tried to read from invalid UART address");
        }
    }
}

void UART8250::storeByte(uint32_t address, uint8_t value) {
    switch (address) {
        case UART_BASE: {
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
        case UART_BASE+1: {
            if (DLABSet()) {
                DLH = value;
            }
            else {
                IER = value;
            }
            break;
        }
        case UART_BASE+2: {
            FCR = value;
            break;
        }
        case UART_BASE+3: {
            LCR = value;
            break;
        }
        case UART_BASE+4: {
            MCR = value;
            break;
        }
        case UART_BASE+5: {
            break;
        }
        case UART_BASE+6: {
            break;
        }
        case UART_BASE+7: {
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