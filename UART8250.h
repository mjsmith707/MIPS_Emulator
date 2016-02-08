//
//  UART8250.h
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef UART8250_h
#define UART8250_h

#include <cstdint>
#include <vector>
#include <iostream>
#include "MMIO_Device.h"

// Also more or less a c&p job from C# version

class UART8250 : public MMIO_Device {
    private:
        // List of port addresses
        std::vector<uint32_t> addresses;
    
        // UART Base address
        #define UART_BASE 0xB40003F8
    
        // Port Registers
        volatile uint8_t THR;
        volatile uint8_t RBR;
        volatile uint8_t DLL;
        volatile uint8_t IER;
        volatile uint8_t DLH;
        volatile uint8_t IIR;
        volatile uint8_t FCR;
        volatile uint8_t LCR;
        volatile uint8_t MCR;
        volatile uint8_t LSR;
        volatile uint8_t MSR;
        volatile uint8_t SR;
    
        // Test if Divisor Latch Access Bit is set
        inline bool DLABSet();
    
    public:
        UART8250();
    
        // Called by MMU to initialize the device
        void initDevice();
    
        // Called by MMU to get address mappings
        std::vector<uint32_t> getAddresses();
    
        // Called by MMU to read a byte
        uint8_t readByte(uint32_t address);
    
        // Called by MMU to store a byte
        void storeByte(uint32_t address, uint8_t value);
    
        // Method for external console access
        void sendChar(char ch);
    
        // Method for external console access
        bool getChar(char* ch);
};

#endif /* UART8250_h */
