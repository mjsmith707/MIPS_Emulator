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
        inline static uint32_t translateVaddr(uint32_t vaddr) {
            
            return vaddr;
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
        inline static uint8_t readByte(uint32_t vaddr) {
            vaddr = translateVaddr(vaddr);
            
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    return itr->second->readByte(vaddr);
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            uint8_t byte = frame[vaddr];
            return byte;
        }
    
        // Read a halfword
        inline static uint16_t readHalf(uint32_t vaddr) {
            vaddr = translateVaddr(vaddr);
            
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    uint16_t half = itr->second->readByte(vaddr);
                    half <<= 8;
                    half |= itr->second->readByte(vaddr+1);
                    return half;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            uint16_t half = frame[vaddr];
            half <<= 8;
            half |= frame[vaddr+1];
            return half;
        }
    
        // Read a word
        inline static uint32_t readWord(uint32_t vaddr) {
            vaddr = translateVaddr(vaddr);
            
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    auto itr = mmioDeviceTable.find(vaddr);
                    uint32_t word = itr->second->readByte(vaddr);
                    word <<= 8;
                    word |= itr->second->readByte(vaddr+1);
                    word <<= 8;
                    word |= itr->second->readByte(vaddr+2);
                    word <<= 8;
                    word |= itr->second->readByte(vaddr+3);
                    return word;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            uint32_t word = frame[vaddr];
            word <<= 8;
            word |= frame[vaddr+1];
            word <<= 8;
            word |= frame[vaddr+2];
            word <<= 8;
            word |= frame[vaddr+3];
            return word;
        }
        
        // Memory writing
        // Store a byte
        inline static void storeByte(uint32_t vaddr, uint8_t value) {
            vaddr = translateVaddr(vaddr);
            
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
        inline static void storeHalf(uint32_t vaddr, uint16_t value) {
            vaddr = translateVaddr(vaddr);
            
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
    
        // Store a word
        inline static void storeWord(uint32_t vaddr, uint32_t value) {
            vaddr = translateVaddr(vaddr);
            
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
