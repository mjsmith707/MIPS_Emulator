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
#include "SharedDefs.h"     // Shared defines
#include "MMIO_Device.h"    // MMIO Device interface
#include "Coprocessor0.h"   // Control Coprocessor Interface
#include "MIPSException.h"  // MIPS Exception Types

// Paged Memory Management Unit
// Singleton class because there really is only ever one of these
class PMMU {
    private:
        // Physical Memory
        // The table of physical memory frames
        static size_t frameTableSize;
        static const size_t pageSize = 4096;
        static const size_t frameTableMax = 0x100000;
        static size_t frameTableRamLimit;
        static uint8_t* frameTable[frameTableMax];
    
        // Memory-Mapped IO
        // Quick linear table to check for mapped addresses
        // Only 32 addresses right now until I can find a faster solution...
        #define MMIOADDRESSTABLEMAX 32
        static uint32_t mmioAddressTable[MMIOADDRESSTABLEMAX];
        static MMIO_Device* mmioDeviceTable[MMIOADDRESSTABLEMAX];
        static uint32_t mmioAddressTableSize;
    
        // TLB and Virtual Memory
    
        // TLB entry
        typedef struct TLBEntry_t {
            uint32_t Mask;
            uint32_t VPN2;
            uint8_t ASID;
            bool G;
            uint32_t PFN0;
            uint8_t C0;
            bool D0;
            bool V0;
            uint32_t PFN1;
            uint8_t C1;
            bool D1;
            bool V1;
        } TLBEntry_t;
    
        // Per CPU TLB entry table
        static TLBEntry_t TLBTable[MAXCPUS][TLBMAXENTRIES];

    
        /* The following translateVaddr functions differ only by the types of exceptions thrown. */
    
        // Virtual to Physical Address translation for data access
        // Also checks address space permissions (i.e. kernel address space accessed from usermode)
        inline static void translateVaddrData(uint32_t* vaddr, Coprocessor0* cop0) {
            // KSEG3
            if (((*vaddr) & 0xE0000000) == 0xE0000000) {
                // Kernel Mapped
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorDataException();
                }
                return;
            }
            // KSSEG
            else if (((*vaddr) & 0xC0000000) == 0xC0000000) {
                // Supervisor Mapped
                // TODO: Supervisor mode is optional and unimplemented
                // so this segment is (correctly) treated as kernel space
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorDataException();
                }
                return;
            }
            // KSEG1
            else if (((*vaddr) & 0xA0000000) == 0xA0000000) {
                // Kernel Unmapped Uncached
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorDataException();
                }
                return;
            }
            // KSEG0
            else if (((*vaddr) & 0x80000000) == 0x80000000) {
                // Kernel Unmapped
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorDataException();
                }
                return;
            }
            // USEG
            else {
                return;
            }
        }
    
        // Virtual to Physical Address translation for instruction fetch
        // Also checks address space permissions (i.e. kernel address space accessed from usermode)
        inline static void translateVaddrIF(uint32_t* vaddr, Coprocessor0* cop0) {
            // KSEG3
            if (((*vaddr) & 0xE0000000) == 0xE0000000) {
                // Kernel Mapped
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorIFException();
                }
                return;
            }
            // KSSEG
            else if (((*vaddr) & 0xC0000000) == 0xC0000000) {
                // Supervisor Mapped
                // TODO: Supervisor mode is optional and unimplemented
                // so this segment is (correctly) treated as kernel space
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorIFException();
                }
                return;
            }
            // KSEG1
            else if (((*vaddr) & 0xA0000000) == 0xA0000000) {
                // Kernel Unmapped Uncached
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorIFException();
                }
                return;
            }
            // KSEG0
            else if (((*vaddr) & 0x80000000) == 0x80000000) {
                // Kernel Unmapped
                // Check ring level
                if (!cop0->inKernelMode()) {
                    throw AddressErrorIFException();
                }
                return;
            }
            // USEG
            else {
                return;
            }
        }
    
        // Retrieves the pointer to the frame block from a physical address
        // otherwise creates a new physical frame entry
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
                        for (uint32_t i=0; i<pageSize; i++) {
                            frameTable[paddr][i] = 0;
                        }
                    #endif
                    frameTableSize++;
                    frame = frameTable[paddr];
                }
            }
            return frame;
        }
    
        /* Alignment Tests */
    
        // Checks if address is word aligned otherwise throws
        // address alignment error for instruction fetch
        inline static void checkWordAlignmentIF(uint32_t vaddr) {
            if ((vaddr & 0x3) > 0) {
                throw AddressErrorIFException();
            }
        }
    
        // Checks if address is word aligned otherwise throws
        // address alignment error for data
        inline static void checkWordAlignmentData(uint32_t vaddr) {
            if ((vaddr & 0x3) > 0) {
                throw AddressErrorDataException();
            }
        }
    
        // Checks if address is halfword aligned otherwise throws
        // address alignment error for data
        inline static void checkHalfAlignmentData(uint32_t vaddr) {
            if ((vaddr & 0x1) > 0) {
                throw AddressErrorDataException();
            }
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
            auto addrtbl = device->getAddresses();
            for (size_t i=0; i<device->getAddresses().size(); i+=2) {
                for (size_t j=0; j<mmioAddressTableSize; j+=2) {
                    if ((addrtbl[i] >= mmioAddressTable[j]) && (addrtbl[i] <= mmioAddressTable[j+1])) {
                        return false;
                    }
                    if ((addrtbl[i+1] >= mmioAddressTable[j]) && (addrtbl[i+1] <= mmioAddressTable[j+1])) {
                        return false;
                    }
                }
            }
            
            // Map the address ranges
            for (size_t i=0; i<device->getAddresses().size(); i+=2) {
                mmioAddressTable[mmioAddressTableSize] = addrtbl[i];
                mmioAddressTable[mmioAddressTableSize+1] = addrtbl[i+1];
                mmioDeviceTable[mmioAddressTableSize] = device;
                mmioDeviceTable[mmioAddressTableSize+1] = device;
                mmioAddressTableSize+=2;
            }
            
            device->initDevice();
            return true;
        }
    
        /*
         * Software/ISA Memory Accessors
         *
         */
    
        // Experimental fast instruction fetch
        inline static void readWordIF(uint32_t vaddr, uint32_t* word) {
            checkWordAlignmentIF(vaddr);
            
            uint32_t* frame = (uint32_t*)getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            frame += vaddr >> 2;
            *word = *frame;
            *word = __builtin_bswap32(*word);
        }
    
        // Memory reading
        // Read a byte
        inline static void readByte(uint32_t vaddr, uint8_t* byte, Coprocessor0* cop0) {
            translateVaddrData(&vaddr, cop0);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *byte = device->readByte(vaddr);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            *byte = frame[vaddr];
        }
    
        // Read a halfword
        inline static void readHalf(uint32_t vaddr, uint16_t* half, Coprocessor0* cop0) {
            checkHalfAlignmentData(vaddr);
            translateVaddrData(&vaddr, cop0);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *half = device->readByte(vaddr);
                    *half <<= 8;
                    *half |= device->readByte(vaddr+1);
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
    
        // Read an unaligned halfword (lwl, lwr)
        inline static void readHalfUnaligned(uint32_t vaddr1, uint32_t vaddr2, uint16_t* half, Coprocessor0* cop0) {
            translateVaddrData(&vaddr1, cop0);
            translateVaddrData(&vaddr2, cop0);
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            MMIO_Device* device1 = nullptr;
            MMIO_Device* device2 = nullptr;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr1 >= mmioAddressTable[i]) && (vaddr1 <= mmioAddressTable[i+1])) {
                    device1 = mmioDeviceTable[i];
                    found1 = true;
                }
                if ((vaddr2 >= mmioAddressTable[i]) && (vaddr2 <= mmioAddressTable[i+1])) {
                    device2 = mmioDeviceTable[i];
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            
            if (found1) {
                *half = device1->readByte(vaddr1);
            }
            else {
                uint8_t* frame1 = getFramePointer(vaddr1);
                vaddr1 &= 0x00000FFF;
                *half = frame1[vaddr1];
            }
            
            if (found2) {
                *half <<= 8;
                *half |= device2->readByte(vaddr2);
            }
            else {
                uint8_t* frame2 = getFramePointer(vaddr2);
                vaddr2 &= 0x00000FFF;
                *half <<= 8;
                *half |= frame2[vaddr2];
            }
        }
    
        // Read a word
        inline static void readWord(uint32_t vaddr, uint32_t* word, Coprocessor0* cop0) {
            checkWordAlignmentData(vaddr);
            translateVaddrData(&vaddr, cop0);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *word = device->readByte(vaddr);
                    *word <<= 8;
                    *word |= device->readByte(vaddr+1);
                    *word <<= 8;
                    *word |= device->readByte(vaddr+2);
                    *word <<= 8;
                    *word |= device->readByte(vaddr+3);
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
        inline static void storeByte(uint32_t vaddr, uint8_t value, Coprocessor0* cop0) {
            translateVaddrData(&vaddr, cop0);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    device->storeByte(vaddr, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(vaddr);
            vaddr &= 0x00000FFF;
            
            frame[vaddr] = value;
        }
    
        // Store a halfword
        inline static void storeHalf(uint32_t vaddr, uint16_t value, Coprocessor0* cop0) {
            checkHalfAlignmentData(vaddr);
            translateVaddrData(&vaddr, cop0);
                // Check if MMIO device holds address
                for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                    if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                        // Found device
                        MMIO_Device* device = mmioDeviceTable[i];
                        device->storeByte(vaddr, value >> 8);
                        device->storeByte(vaddr+1, value);
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
        inline static void storeHalfUnaligned(uint32_t vaddr1, uint32_t vaddr2, uint16_t value, Coprocessor0* cop0) {
            translateVaddrData(&vaddr1, cop0);
            translateVaddrData(&vaddr2, cop0);
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            MMIO_Device* device1 = nullptr;
            MMIO_Device* device2 = nullptr;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr1 >= mmioAddressTable[i]) && (vaddr1 <= mmioAddressTable[i+1])) {
                    device1 = mmioDeviceTable[i];
                    found1 = true;
                }
                if ((vaddr2 >= mmioAddressTable[i]) && (vaddr2 <= mmioAddressTable[i+1])) {
                    device2 = mmioDeviceTable[i];
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            if (found1) {
                device1->storeByte(vaddr1, value >> 8);
            }
            else {
                uint8_t* frame1 = getFramePointer(vaddr1);
                vaddr1 &= 0x00000FFF;
                frame1[vaddr1] = value >> 8;
            }
            
            if (found2) {
                device2->storeByte(vaddr2, value);
            }
            else {
                uint8_t* frame2 = getFramePointer(vaddr2);
                vaddr2 &= 0x00000FFF;
                frame2[vaddr2] = value;
            }

        }
    
        // Store a word
        inline static void storeWord(uint32_t vaddr, uint32_t value, Coprocessor0* cop0) {
            checkWordAlignmentData(vaddr);
            translateVaddrData(&vaddr, cop0);
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((vaddr >= mmioAddressTable[i]) && (vaddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    device->storeByte(vaddr, value >> 24);
                    device->storeByte(vaddr+1, value >> 16);
                    device->storeByte(vaddr+2, value >> 8);
                    device->storeByte(vaddr+3, value);
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
    
       /*
        *   TLB Instructions
        *
        */
    
        // TLBP
        // Probes the TLB for a matching entry and updates CO0_Index Register
        // with the result of the search.
        inline static void probeTLB(uint8_t cpuNum, Coprocessor0* cop0) {
            // Set Index bit 31 to 1
            cop0->setRegisterHW(CO0_INDEX, 0x1u << 31);
            
            for (uint8_t i=0; i<TLBMAXENTRIES; i++) {
                // There has to be a way to simplify this...
                // Pg 268 vol 3
                if (((TLBTable[cpuNum][i].VPN2 & ~TLBTable[cpuNum][i].Mask) == (cop0->getRegister(CO0_ENTRYHI) & ~TLBTable[cpuNum][i].Mask))
                    && (TLBTable[cpuNum][i].G || (TLBTable[cpuNum][i].ASID == (cop0->getRegister(CO0_ENTRYHI) & ENTRYHI_ASID)))) {
                    
                    // Update CO0_Index
                    cop0->setRegisterHW(CO0_INDEX, i);
                    break;
                }
            }
        }
    
        // TLBR
        // Reads the TLB entry specified by CO0_Index Register
        inline static void readTLB(uint8_t cpuNum, Coprocessor0* cop0) {
            // Check ring level
            if (!cop0->inKernelMode()) {
                throw CoprocessorUnusableException(CoprocessorUnusableException::FaultingCoprocessor::CO0);
            }
            
            uint8_t i = cop0->getRegister(CO0_INDEX);
            if (i >= TLBMAXENTRIES) {
                // Undefined
                return;
            }
            
            // Update PageMask
            cop0->andRegisterHW(CO0_PAGEMASK, ~PAGEMASK_MASK);
            cop0->orRegisterHW(CO0_PAGEMASK, TLBTable[cpuNum][i].Mask);
            
            // Update EntryHi
            cop0->setRegisterHW(CO0_ENTRYHI, (TLBTable[cpuNum][i].VPN2 & ~TLBTable[cpuNum][i].Mask) | TLBTable[cpuNum][i].ASID);
            
            // Update EntryLo1
            cop0->setRegisterHW(CO0_ENTRYLO1, (TLBTable[cpuNum][i].PFN1 & ~TLBTable[cpuNum][i].Mask)
                                  | (TLBTable[cpuNum][i].C1 << 3) | (TLBTable[cpuNum][i].D1 << 2) | (TLBTable[cpuNum][i].V1 << 1)
                                  | (TLBTable[cpuNum][i].G));
            
            // Update EntryLo0
            cop0->setRegisterHW(CO0_ENTRYLO0, (TLBTable[cpuNum][i].PFN0 & ~TLBTable[cpuNum][i].Mask)
                                  | (TLBTable[cpuNum][i].C0 << 3) | (TLBTable[cpuNum][i].D0 << 2) | (TLBTable[cpuNum][i].V0 << 1)
                                  | (TLBTable[cpuNum][i].G));
        }
    
        // TLBWI
        // Writes a TLB entry to the value of CO0_Index Register
        inline static void writeIndexedTLB(uint8_t cpuNum, Coprocessor0* cop0) {
            // Check ring level
            if (!cop0->inKernelMode()) {
                throw CoprocessorUnusableException(CoprocessorUnusableException::FaultingCoprocessor::CO0);
            }
            
            uint8_t i = cop0->getRegister(CO0_INDEX);
            if (i >= TLBMAXENTRIES) {
                // Undefined
                return;
            }
            
            // Insert TLB Entries
            TLBTable[cpuNum][i].Mask = cop0->getRegister(CO0_PAGEMASK) & PAGEMASK_MASK;
            TLBTable[cpuNum][i].VPN2 = cop0->getRegister(CO0_ENTRYHI) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].ASID = cop0->getRegister(CO0_ENTRYHI) & ENTRYHI_ASID;
            TLBTable[cpuNum][i].G = ((cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_G) > 0) && ((cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_G) > 0);
            TLBTable[cpuNum][i].PFN1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_PFN) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].C1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_C);
            TLBTable[cpuNum][i].D1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_D) > 0;
            TLBTable[cpuNum][i].V1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_V) > 0;
            TLBTable[cpuNum][i].PFN0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_PFN) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].C0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_C);
            TLBTable[cpuNum][i].D0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_D) > 0;
            TLBTable[cpuNum][i].V0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_V) > 0;
        }
    
        // TLBWR
        // Writes a TLB entry to the value of CO0_Random Register
        inline static void writeRandomTLB(uint8_t cpuNum, Coprocessor0* cop0) {
            // Check ring level
            if (!cop0->inKernelMode()) {
                throw CoprocessorUnusableException(CoprocessorUnusableException::FaultingCoprocessor::CO0);
            }
            
            // Get random index
            uint8_t i = cop0->getRegister(CO0_RANDOM);
            
            // Insert TLB Entries
            TLBTable[cpuNum][i].Mask = cop0->getRegister(CO0_PAGEMASK) & PAGEMASK_MASK;
            TLBTable[cpuNum][i].VPN2 = cop0->getRegister(CO0_ENTRYHI) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].ASID = cop0->getRegister(CO0_ENTRYHI) & ENTRYHI_ASID;
            TLBTable[cpuNum][i].G = ((cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_G) > 0) && ((cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_G) > 0);
            TLBTable[cpuNum][i].PFN1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_PFN) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].C1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_C);
            TLBTable[cpuNum][i].D1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_D) > 0;
            TLBTable[cpuNum][i].V1 = (cop0->getRegister(CO0_ENTRYLO1) & ENTRYLO1_V) > 0;
            TLBTable[cpuNum][i].PFN0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_PFN) & ~TLBTable[cpuNum][i].Mask;
            TLBTable[cpuNum][i].C0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_C);
            TLBTable[cpuNum][i].D0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_D) > 0;
            TLBTable[cpuNum][i].V0 = (cop0->getRegister(CO0_ENTRYLO0) & ENTRYLO0_V) > 0;
        }
    
       /*
        * Physical Memory Accessors
        *
        * For use by non ISA code, e.g. ELF loaders
        * unit tests etc. Do not throw errors for alignment
        * or address translation.
        */
    
        // Memory reading
        // Read a byte
        inline static void readBytePhys(uint32_t paddr, uint8_t* byte) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *byte = device->readByte(paddr);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            *byte = frame[paddr];
        }
        
        // Read a halfword
        inline static void readHalfPhys(uint32_t paddr, uint16_t* half) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *half = device->readByte(paddr);
                    *half <<= 8;
                    *half |= device->readByte(paddr+1);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            *half = frame[paddr];
            *half <<= 8;
            *half |= frame[paddr+1];
        }
        
        // Read an unaligned halfword (lwl, lwr)
        inline static void readHalfUnalignedPhys(uint32_t paddr1, uint32_t paddr2, uint16_t* half) {
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            MMIO_Device* device1 = nullptr;
            MMIO_Device* device2 = nullptr;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr1 >= mmioAddressTable[i]) && (paddr1 <= mmioAddressTable[i+1])) {
                    device1 = mmioDeviceTable[i];
                    found1 = true;
                }
                if ((paddr2 >= mmioAddressTable[i]) && (paddr2 <= mmioAddressTable[i+1])) {
                    device2 = mmioDeviceTable[i];
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            
            if (found1) {
                *half = device1->readByte(paddr1);
            }
            else {
                uint8_t* frame1 = getFramePointer(paddr1);
                paddr1 &= 0x00000FFF;
                *half = frame1[paddr1];
            }
            
            if (found2) {
                *half <<= 8;
                *half |= device2->readByte(paddr2);
            }
            else {
                uint8_t* frame2 = getFramePointer(paddr2);
                paddr2 &= 0x00000FFF;
                *half <<= 8;
                *half |= frame2[paddr2];
            }
        }
        
        // Read a word
        inline static void readWordPhys(uint32_t paddr, uint32_t* word) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    *word = device->readByte(paddr);
                    *word <<= 8;
                    *word |= device->readByte(paddr+1);
                    *word <<= 8;
                    *word |= device->readByte(paddr+2);
                    *word <<= 8;
                    *word |= device->readByte(paddr+3);
                    return;
                }
            }
            
            // No MMIO Device found so retrieve from memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            *word = frame[paddr];
            *word <<= 8;
            *word |= frame[paddr+1];
            *word <<= 8;
            *word |= frame[paddr+2];
            *word <<= 8;
            *word |= frame[paddr+3];
        }
        
        // Memory writing
        // Store a byte
        inline static void storeBytePhys(uint32_t paddr, uint8_t value) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    device->storeByte(paddr, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            frame[paddr] = value;
        }
        
        // Store a halfword
        inline static void storeHalfPhys(uint32_t paddr, uint16_t value) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    device->storeByte(paddr, value >> 8);
                    device->storeByte(paddr+1, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            frame[paddr] = value >> 8;
            frame[paddr+1] = value;
        }
        
        // Store an unaligned halfword (swl, swr)
        inline static void storeHalfUnalignedPhys(uint32_t paddr1, uint32_t paddr2, uint16_t value, Coprocessor0* cop0) {
            // Check if MMIO device holds address
            bool found1 = false;
            bool found2 = false;
            MMIO_Device* device1 = nullptr;
            MMIO_Device* device2 = nullptr;
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr1 >= mmioAddressTable[i]) && (paddr1 <= mmioAddressTable[i+1])) {
                    device1 = mmioDeviceTable[i];
                    found1 = true;
                }
                if ((paddr2 >= mmioAddressTable[i]) && (paddr2 <= mmioAddressTable[i+1])) {
                    device2 = mmioDeviceTable[i];
                    found2 = true;
                }
                if (found1 && found2) {
                    break;
                }
            }
            if (found1) {
                device1->storeByte(paddr1, value >> 8);
            }
            else {
                uint8_t* frame1 = getFramePointer(paddr1);
                paddr1 &= 0x00000FFF;
                frame1[paddr1] = value >> 8;
            }
            
            if (found2) {
                device2->storeByte(paddr2, value);
            }
            else {
                uint8_t* frame2 = getFramePointer(paddr2);
                paddr2 &= 0x00000FFF;
                frame2[paddr2] = value;
            }
            
        }
        
        // Store a word
        inline static void storeWordPhys(uint32_t paddr, uint32_t value) {
            // Check if MMIO device holds address
            for (uint32_t i=0; i+1<mmioAddressTableSize; i+=2) {
                if ((paddr >= mmioAddressTable[i]) && (paddr <= mmioAddressTable[i+1])) {
                    // Found device
                    MMIO_Device* device = mmioDeviceTable[i];
                    device->storeByte(paddr, value >> 24);
                    device->storeByte(paddr+1, value >> 16);
                    device->storeByte(paddr+2, value >> 8);
                    device->storeByte(paddr+3, value);
                    return;
                }
            }
            
            // No MMIO Device found so save to memory
            uint8_t* frame = getFramePointer(paddr);
            paddr &= 0x00000FFF;
            
            frame[paddr] = value >> 24;
            frame[paddr+1] = value >> 16;
            frame[paddr+2] = value >> 8;
            frame[paddr+3] = value;
        }

};

#endif /* PMMU_h */
