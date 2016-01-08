//
//  MMU.h
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef PMMU_h
#define PMMU_h

#include <cstdint>          // uint32_t
#include <iostream>         // std::cerr

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
    
        // Memory reading
        // All reads/writes are per byte so if an address sits on a
        // frame boundary it wont matter
        // Read a byte
        inline static uint8_t readByte(uint32_t vaddr) {
            uint32_t paddr = translateVaddr(vaddr);
            uint8_t* frame = getFramePointer(paddr);
            uint8_t byte = frame[paddr&0x00000FFF];
            return byte;
        }
    
        // Read a halfword
        inline static uint16_t readHalf(uint32_t vaddr) {
            uint16_t half = readByte(vaddr);
            half <<= 8;
            half |= readByte(vaddr+1);
            return half;
        }
    
        // Read a word
        inline static uint32_t readWord(uint32_t vaddr) {
            uint32_t word = readByte(vaddr);
            word <<= 8;
            word |= readByte(vaddr+1);
            word <<= 8;
            word |= readByte(vaddr+2);
            word <<= 8;
            word |= readByte(vaddr+3);
            return word;
        }
        
        // Memory writing
        // Store a byte
        inline static void storeByte(uint32_t vaddr, uint8_t value) {
            uint32_t paddr = translateVaddr(vaddr);
            uint8_t* frame = getFramePointer(paddr);
            frame[paddr&0x00000FFF] = value;
        }
    
        // Store a halfword
        inline static void storeHalf(uint32_t vaddr, uint16_t value) {
            storeByte(vaddr, value >> 8);
            storeByte(vaddr+1, value);
        }
    
        // Store a word
        inline static void storeWord(uint32_t vaddr, uint32_t value) {
            storeByte(vaddr, value >> 24);
            storeByte(vaddr+1, value >> 16);
            storeByte(vaddr+2, value >> 8);
            storeByte(vaddr+3, value);
        }
    
};

#endif /* PMMU_h */
