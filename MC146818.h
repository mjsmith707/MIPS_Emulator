//
//  MC146818.h
//  MIPS_Emulator
//
//  Created by Matt on 5/12/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef MC146818_h
#define MC146818_h

#include "Device.h"
#include "MMIO_Device.h"
#include "Clockable_Device.h"
#include "Interruptable_Device.h"
#include <string>
#include <ctime>
#include <thread>
#include <atomic>

class MC146818 : public Device, public MMIO_Device, public Clockable_Device, public Interruptable_Device {
    private:
        // List of port addresses
        std::vector<uint32_t> addresses;
        
        //#define CMOS_BASE 0x14000070
        
        // Also map to 0
        #define CMOS_BASEPHYS 0x70u
    
        // Address offset
        const uint32_t addrOffset = 0x0;
    
        // 64 Byte array
        uint8_t registers[64];
    
        // Time variables
        time_t utctime;
        struct tm* ptm;
    
        // CPU Handle for interrupts
        CPU* cpu;
    
        // Changing interval to signal clock updates
        uint32_t cycleInterval;
    
        // Updates clock registers
        void updateClock();
    
    public:
        // Constructor
        MC146818();
    
        // Destructor
        ~MC146818();
    
        /*
         *  MMIO Device Interface
         */
    
        // Called by MMU to initialize the device
        void initDevice();
        
        // Called by MMU to get address mappings
        std::vector<uint32_t> getAddresses();
        
        // Called by MMU to read a byte
        uint8_t readByte(uint32_t address);
        
        // Called by MMU to store a byte
        void storeByte(uint32_t address, uint8_t value);
    
        /*
         *  Clockable Device Interface
         */
    
        // Used to get the interval between executions
        uint32_t getCycleInterval();
    
        // Indicates to the CPU that the interval is enabled
        bool isEnabled();
    
        // Called by CPU when cycle interval is reached
        void onCycleExecute();
    
        /*
         *  Interruptable Device Interface
         */
    
        // Gets a pointer to the CPU so it can call sendInterrupt etc.
        void getCPUHandle(CPU* cpu);
};

#endif /* MC146818_h */
