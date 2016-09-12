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
#include <atomic>
#include "Device.h"
#include "MMIO_Device.h"
#include "SharedQueue.h"

// Also more or less a c&p job from C# version

class UART8250 : public Device, public MMIO_Device {
    private:
        // List of port addresses
        std::vector<uint32_t> addresses;
    
        // UART Base address
        // This was the address before 512mb phys region mapping
        //#define UART_BASE 0xB40003F8
        // This is it now
        #define UART_BASE 0x140003F8
    
        // Also map to 0
        #define UART_BASEPHYS 0x3F8
    
        const uint32_t addrOffset = 0x8;
    
        // Receive Buffer
        Shared_Queue<char> receiveBuffer;
    
        // Send Buffer
        Shared_Queue<char> sendBuffer;
    
        // Port Registers
        std::atomic<uint8_t> THR;
        std::atomic<uint8_t> RBR;
        std::atomic<uint8_t> DLL;
        std::atomic<uint8_t> IER;
        std::atomic<uint8_t> DLH;
        std::atomic<uint8_t> IIR;
        std::atomic<uint8_t> FCR;
        std::atomic<uint8_t> LCR;
        std::atomic<uint8_t> MCR;
        std::atomic<uint8_t> LSR;
        std::atomic<uint8_t> MSR;
        std::atomic<uint8_t> SR;
    
        // Test if Divisor Latch Access Bit is set
        inline bool DLABSet();
    
    public:
        UART8250();
    
        ~UART8250();
    
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
