//
//  SoftRes.hpp
//  MIPS_Emulator
//
//  Created by Matt on 7/10/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef SoftRes_h
#define SoftRes_h

#include "Device.h"
#include "MMIO_Device.h"
#include "MIPSException.h"

// MMIO Register which resets the machine when
// written to with a special value
class SoftRes : public Device, public MMIO_Device {
    private:
        // List of port addresses
        std::vector<uint32_t> addresses;
    
        // Base address for SoftRes and BrkRes
        //#define SOFTRES_BASE 0xBF000500
        #define SOFTRES_BASE 0x1F000500
        #define SOFTRES_PHYBASE 0x500
    
        // Magic number which triggers a reset
        const uint8_t GORESET = 0x42;
    
        // SoftRes Register
        uint8_t registers[12];
    
    public:
        SoftRes();
        ~SoftRes();
    
        void initDevice();
        std::vector<uint32_t> getAddresses();
        uint8_t readByte(uint32_t address);
        void storeByte(uint32_t address, uint8_t value);
};


#endif /* SoftRes_h */
