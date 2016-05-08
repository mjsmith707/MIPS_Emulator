//
//  main.cpp
//  MIPS_Emulator
//
//  Created by Matt on 12/30/15.
//  Copyright © 2015 Matt. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <thread>
#include <sstream>
#include <fstream>

#include "PMMU.h"
#include "CPU.h"
#include "UART8250.h"
#include "ConsoleUI.h"
#include "elfio/elfio.hpp"

void loadRaw(ConsoleUI*, const char*, PMMU*, CPU*);
void loadFile(ConsoleUI*, const char*, PMMU*, CPU*);
void setBootArgs(ConsoleUI*, const char*, PMMU*, CPU*);

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        std::cout << "Usage ./MIPS_Emulator <elf_filename>" << std::endl;
        exit(0);
    }
    
    // Create memory management unit
    // Roughly 4GB
    PMMU* memory = new PMMU(1000000);
    
    // Create uart8250
    UART8250* uart8250 = new UART8250();
    
    // Attach uart to iommu
    memory->attachDevice(uart8250);
    
    // Create ConsoleUI
    ConsoleUI* consoleUI = new ConsoleUI(uart8250);
    
    // Create cpu
    CPU* cpu0 = new CPU(0, consoleUI, memory);
    
    // Start paused
    cpu0->sendThreadSignal(CPU::PAUSE);
    
    // Attach cpu to ConsoleUI
    consoleUI->attachCPU(cpu0);
    
    // Attach memory to ConsoleUI
    consoleUI->attachMemory(memory);
    
    // Load binary
    loadFile(consoleUI, argv[1], memory, cpu0);
    
    // Start CPU
    std::thread cpu0_thread(std::bind(&CPU::start, cpu0));
    
    // Wait for console input
    consoleUI->waitForInput();
    
    // Stop CPU
    cpu0->sendThreadSignal(CPU::HALT);
    cpu0_thread.join();
    
    // Cleanup
    delete consoleUI;
    delete uart8250;
    delete cpu0;
    delete memory;
    
    return 0;
}

// Loads a flat binary file and sets program counter to reset exception vector 0xbfc00000
void loadRaw(ConsoleUI* consoleUI, const char* filename, PMMU* memory, CPU* cpu) {
    std::fstream fhandle;
    fhandle.open(filename, std::ios::in | std::ios::binary);
    if (!fhandle.is_open()) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        exit(-1);
    }
    
    fhandle.seekg(0, fhandle.end);
    size_t len = fhandle.tellg();
    fhandle.seekg(0, fhandle.beg);
    
    char* buff = new char[len];
    
    fhandle.read(buff, len);
    if (!fhandle) {
        std::cerr << "Failed to read file: " << filename << ", only read " << fhandle.gcount() << " bytes" << std::endl;
        exit(-1);
    }
    
    uint32_t paddr = 0xbfc00000;
    for (size_t i=0; i<len; i++, paddr++) {
        memory->storeBytePhys(paddr, buff[i]);
    }
    cpu->setPC(0xbfc00000);
}

// Loads ELF file formats
// ... it's also not 100% working
void loadFile(ConsoleUI* consoleUI, const char* filename, PMMU* memory, CPU* cpu) {
    ELFIO::elfio reader;
    if (!reader.load(filename)) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        exit(-1);
    }
    
    // Print ELF file properties
    std::stringstream ss;
    ss << "ELF file class    : ";
    if ( reader.get_class() == ELFCLASS32 )
        ss << "ELF32" << std::endl;
    else
        ss << "ELF64" << std::endl;
    
    ss << "ELF file encoding : ";
    if ( reader.get_encoding() == ELFDATA2LSB )
        ss << "Little endian" << std::endl;
    else
        ss << "Big endian" << std::endl;
    
    // Set Program Counter to ELF Entry Point
    ss << std::hex << "ELF Entry Point: 0x" << reader.get_entry() << std::endl;
    cpu->setPC((uint32_t)reader.get_entry());
    
    // Print ELF file sections info
    ELFIO::Elf_Half sec_num = reader.sections.size();
    ss << "Number of sections: " << sec_num << std::endl;
    for ( int i = 0; i < sec_num; ++i ) {
        ELFIO::section* psec = reader.sections[i];
        ss << "  [" << i << "] "
        << psec->get_name()
        << "\t"
        << psec->get_size()
        << std::endl;
        // Access to section's data
        // const char* p = reader.sections[i]->get_data()
    }
    
    // Print ELF file segments info
    ELFIO::Elf_Half seg_num = reader.segments.size();
    ss << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
        const ELFIO::segment* pseg = reader.segments[i];
        ss << "  [" << i << "] 0x" << std::hex
        << pseg->get_flags()
        << "\t0x"
        << pseg->get_virtual_address()
        << "\t0x"
        << pseg->get_file_size()
        << "\t0x"
        << pseg->get_memory_size()
        << std::endl;
        
        // Access to segments's data
        const char* p = pseg->get_data();
        if (p != NULL) {
            // Load segment into memory
            uint32_t addr = (uint32_t)pseg->get_virtual_address();
            for (uint32_t j=0; j < pseg->get_file_size(); j++, addr++) {
                memory->storeBytePhys(addr, p[j]);
            }
        }
    }
    consoleUI->sendConsoleMsg(ss.str());
    //std::cout << ss.str() << std::endl;
}

// Very much a work in progress. I'm still not sure how Linux actually expects these arguments to be.
void setBootArgs(ConsoleUI* consoleUI, const char* str, PMMU* memory, CPU* cpu) {
    // Store in some low area of memory
    uint32_t baseaddr = 0xA0001000;
    uint32_t addr = baseaddr;
    uint32_t count = 1;
    while (*str) {
        memory->storeBytePhys(addr, *str);
        str++;
        addr++;
        if (*str == ' ') {
            count++;
        }
    }
    cpu->setRegister(4, count);
    cpu->setRegister(5, baseaddr);
    cpu->setRegister(6, 0);
    cpu->setRegister(7, 0);
}
