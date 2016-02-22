//
//  PMMU.h
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef PMMU_h
#define PMMU_h

#include <cstdint>          // uint32_t
#include <iostream>         // std::cerr
#include <unordered_map>    // hash table (and soooo slowwwwww)
#include "MMIO_Device.h"    // MMIO Device interface
#include "Coprocessor0.h"   // Control Coprocessor Interface

// Paged Memory Management Unit
// Singleton class because there really is only ever one of these
class PMMU {
    private:
        // The table of physical memory frames
        static size_t frameTableSize;
        static const size_t pageSize = 4096;
        static const size_t frameTableMax = 0x100000;
        static size_t frameTableRamLimit;
        static uint8_t* frameTable[frameTableMax];
        // Quick linear table to check for mapped addresses
        // Only 32 addresses right now until I can find a faster solution...
        #define MMIOADDRESSTABLEMAX 32
        static uint32_t mmioAddressTable[MMIOADDRESSTABLEMAX];
        static uint32_t mmioAddressTableSize;
        static std::unordered_map<uint32_t, MMIO_Device*> mmioDeviceTable;
    
        // Virtual to Physical Address translation
        // Returns true on success
        // False on failure (address error etc.)
        inline static void translateVaddr(uint32_t* vaddr, Coprocessor0* coproc) {
            // KSEG3
            if (((*vaddr) & 0xE0000000) == 0xE0000000) {
                // Kernel Mapped
                return;
            }
            // KSSEG
            else if (((*vaddr) & 0xC0000000) == 0xC0000000) {
                // Supervisor Mapped
                return;
            }
            // KSEG1
            else if (((*vaddr) & 0xA0000000) == 0xA0000000) {
                // Kernel Unmapped Uncached
                return;
            }
            // KSEG0
            else if (((*vaddr) & 0x80000000) == 0x80000000) {
                // Kernel Unmapped
                return;
            }
            // USEG
            else {
                return;
            }
        }
    
        // Retrieves the pointer to the frame block from a physical address
        inline static uint8_t* getFramePointer(uint32_t paddr) {
            // Get the frame pointer
            paddr = (paddr&0xFFFFF000) >> 12;
            uint8_t* frame = frameTable[paddr];
            
            if (frame == NULL) {
                // Check if out of memory
                if (frameTableSize == frameTableRamLimit) {
                    // Probably fatal for the VM
                    std::cerr << "MMU has run out of memory!" << std::endl;
                    exit(-1);
                }
                else {
                    // Allocate a new physical frame
                    frameTable[paddr] = new uint8_t[pageSize];
                    #ifdef TEST_PROJECT
                        // Zero memory for testing
                        // Disabled for performance reasons during
                        // normal use as it is not guaranteed
                        // to be all zeroes
                        for (int i=0; i<pageSize; i++) {
                            frameTable[paddr][i] = 0;
                        }
                    #endif
                    frameTableSize++;
                    frame = frameTable[paddr];
                }
            }
            return frame;
        }
    
    public:
        PMMU(size_t ramSize);
        ~PMMU();
    
        // Attaches a device to its given range of addresses
        // and calls its initializer
        // The device addresses vector is a pair of start/end addresses
        static bool attachDevice(MMIO_Device* device) {
            if (device->getAddresses().size() % 2 > 0) {
                throw std::runtime_error("Error: MMIO Device addresses must be a pair of ranges!");
            }
            
            // Check if address range was already mapped
            for (size_t i=0; i<device->getAddresses().size(); i+=2) {
                for (uint32_t addr=device->getAddresses()[i]; addr<=device->getAddresses()[i+1]; addr++) {
                    auto itr = mmioDeviceTable.find(addr);
                    if (itr != mmioDeviceTable.end()) {
                        return false;
                    }
                }
            }
            
            // Map the address ranges
            for (size_t i=0; i<device->getAddresses().size(); i+=2) {
                for (uint32_t addr=device->getAddresses()[i]; addr<=device->getAddresses()[i+1]; addr++) {
                    mmioDeviceTable.emplace(addr, device);
                }
                mmioAddressTable[mmioAddressTableSize] = device->getAddresses()[i];
                mmioAddressTable[mmioAddressTableSize+1] = device->getAddresses()[i+1];
                mmioAddressTableSize+=2;
            }
            
            device->initDevice();
            return true;
        }
    
        // Memory reading
        // Read a byte
        inline static void readByte(uint32_t vaddr, uint8_t* byte, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    *byte = itr->second->readByte(vaddr);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            *byte = frame[vaddr];
        }
    
        // Read a halfword
        inline static void readHalf(uint32_t vaddr, uint16_t* half, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    *half = itr->second->readByte(vaddr);
                    *half <<= 8;
                    *half |= itr->second->readByte(vaddr+1);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            *half = frame[vaddr];
            *half <<= 8;
            *half |= frame[vaddr+1];
        }
    
        // Read an unaligned halfword (lwl, lwr)23e  cxerf a
        inline static void readHalfUnaligned(uint32_t vaddr1, uint32_t vaddr2, uint16_t* half, Coprocessor0* coproc) {
            translateVaddr(&vaddr1, coproc);
            translateVaddr(&vaddr2, coproc);
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr1 >= mmioAddressTable[i]) && (vaddr1 <= mmioAddressTable[i+1])) {
                    found1 = true;
                }
                if ((vaddr2 >= mmioAddressTable[i]) && (vaddr2 <= mmioAddressTable[i+1])) {
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            
            if (found1) {
                auto itr1 = mmioDeviceTable.find(vaddr1);
                *half = itr1->second->readByte(vaddr1);
            }
            else {
                uint8_t* frame1 = getFramePointer(vaddr1);
                vaddr1 &= 0x00000FFF;
                *half = frame1[vaddr1];
            }
            
            if (found2) {
                auto itr2 = mmioDeviceTable.find(vaddr2);
                *half <<= 8;
                *half |= itr2->second->readByte(vaddr2);
            }
            else {
                uint8_t* frame2 = getFramePointer(vaddr2);
                vaddr2 &= 0x00000FFF;
                *half <<= 8;
                *half |= frame2[vaddr2];
            }
        }
    
        // Read a word
        inline static void readWord(uint32_t vaddr, uint32_t* word, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    *word = itr->second->readByte(vaddr);
                    *word <<= 8;
                    *word |= itr->second->readByte(vaddr+1);
                    *word <<= 8;
                    *word |= itr->second->readByte(vaddr+2);
                    *word <<= 8;
                    *word |= itr->second->readByte(vaddr+3);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            *word = frame[vaddr];
            *word <<= 8;
            *word |= frame[vaddr+1];
            *word <<= 8;
            *word |= frame[vaddr+2];
            *word <<= 8;
            *word |= frame[vaddr+3];
        }
        
        // Memory writing
        // Store a byte
        inline static void storeByte(uint32_t vaddr, uint8_t value, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    itr->second->storeByte(vaddr, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            frame[vaddr] = value;
        }
    
        // Store a halfword
        inline static void storeHalf(uint32_t vaddr, uint16_t value, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
                // Check if MMIO device holds address
                for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                    if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                        // Found device
                        auto itr = mmioDeviceTable.find(vaddr);
                        itr->second->storeByte(vaddr, value >> 8);
                        itr->second->storeByte(vaddr+1, value);
                        return;
                    }
                }
                
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            frame[vaddr] = value >> 8;
            frame[vaddr+1] = value;
        }
    
        // Store an unaligned halfword (swl, swr)
        inline static void storeHalfUnaligned(uint32_t vaddr1, uint32_t vaddr2, uint16_t value, Coprocessor0* coproc) {
            translateVaddr(&vaddr1, coproc);
            translateVaddr(&vaddr2, coproc);
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr1 >= mmioAddressTable[i]) && (vaddr1 <= mmioAddressTable[i+1])) {
                    found1 = true;
                }
                if ((vaddr2 >= mmioAddressTable[i]) && (vaddr2 <= mmioAddressTable[i+1])) {
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            if (found1) {
                auto itr1 = mmioDeviceTable.find(vaddr1);
                itr1->second->storeByte(vaddr1, value >> 8);
            }
            else {
                uint8_t* frame1 = getFramePointer(vaddr1);
                vaddr1 &= 0x00000FFF;
                frame1[vaddr1] = value >> 8;
            }
            
            if (found2) {
                auto itr2 = mmioDeviceTable.find(vaddr2);
                itr2->second->storeByte(vaddr2, value);
            }
            else {
                uint8_t* frame2 = getFramePointer(vaddr2);
                vaddr2 &= 0x00000FFF;
                frame2[vaddr2] = value;
            }

        }
    
        // Store a word
        inline static void storeWord(uint32_t vaddr, uint32_t value, Coprocessor0* coproc) {
            translateVaddr(&vaddr, coproc);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    itr->second->storeByte(vaddr, value >> 24);
                    itr->second->storeByte(vaddr+1, value >> 16);
                    itr->second->storeByte(vaddr+2, value >> 8);
                    itr->second->storeByte(vaddr+3, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            frame[vaddr] = value >> 24;
            frame[vaddr+1] = value >> 16;
            frame[vaddr+2] = value >> 8;
            frame[vaddr+3] = value;
        }
};

#endif /* PMMU_h */
