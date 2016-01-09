//
//  MMIO_Device.h
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef MMIO_Device_h
#define MMIO_Device_h

// Interface for memory mapped io devices to attach to PMMU

#include <cstdint>
#include <vector>

class MMIO_Device {
    public:
        virtual void initDevice() = 0;
        virtual std::vector<uint32_t> getAddresses() = 0;
        virtual uint8_t readByte(uint32_t address) = 0;
        virtual void storeByte(uint32_t address, uint8_t value) = 0;
};

#endif /* MMIO_Device_h */
