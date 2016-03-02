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

#include "PMMU.h"
#include "CPU.h"
#include "UART8250.h"
#include "ConsoleUI.h"
#include "elfio/elfio.hpp"

void loadFile(ConsoleUI*, const char*, PMMU*, CPU*);
void waitForInput(UART8250* uart);

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
    CPU* cpu0 = new CPU(consoleUI, memory);
    
    // Load binary
    loadFile(consoleUI, argv[1], memory, cpu0);
    
    // Start CPU
    std::thread cpu0_thread(std::bind(&CPU::start, cpu0));
    //std::this_thread::sleep_for(std::chrono::seconds(60));
    
    // Wait for console input
    consoleUI->waitForInput();
    
    // Stop CPU
    cpu0->sendInterrupt(CPU::MIPSInterrupt::HALT);
    cpu0_thread.join();
    
    // Cleanup
    delete consoleUI;
    delete uart8250;
    delete cpu0;
    delete memory;
    
    return 0;
}

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
                memory->storeByte(addr, p[j], cpu->getControlCoprocessor());
            }
        }
    }
}