//
//  main.cpp
//  MIPS_Emulator
//
//  Created by Matt on 12/30/15.
//  Copyright © 2015 Matt. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "PMMU.h"
#include "CPU.h"
#include "elfio/elfio.hpp"

void loadFile(const char*, PMMU*, CPU*);

int main(int argc, const char * argv[]) {
    PMMU* memory = new PMMU(1000000);
    CPU* cpu0 = new CPU(memory);
    loadFile("mmon.elf", memory, cpu0);
    cpu0->start();
    return 0;
}

void loadFile(const char* filename, PMMU* memory, CPU* cpu) {
    ELFIO::elfio reader;
    if (!reader.load(filename)) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        exit(-1);
    }
    
    // Print ELF file properties
    std::cout << "ELF file class    : ";
    if ( reader.get_class() == ELFCLASS32 )
        std::cout << "ELF32" << std::endl;
    else
        std::cout << "ELF64" << std::endl;
    
    std::cout << "ELF file encoding : ";
    if ( reader.get_encoding() == ELFDATA2LSB )
        std::cout << "Little endian" << std::endl;
    else
        std::cout << "Big endian" << std::endl;
    
    // Set Program Counter to ELF Entry Point
    std::cout << std::hex << "ELF Entry Point: 0x" << reader.get_entry() << std::endl;
    cpu->setPC((uint32_t)reader.get_entry());
    
    // Print ELF file sections info
    ELFIO::Elf_Half sec_num = reader.sections.size();
    std::cout << "Number of sections: " << sec_num << std::endl;
    for ( int i = 0; i < sec_num; ++i ) {
        ELFIO::section* psec = reader.sections[i];
        std::cout << "  [" << i << "] "
        << psec->get_name()
        << "\t"
        << psec->get_size()
        << std::endl;
        // Access to section's data
        // const char* p = reader.sections[i]->get_data()
    }
    
    // Print ELF file segments info
    ELFIO::Elf_Half seg_num = reader.segments.size();
    std::cout << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
        const ELFIO::segment* pseg = reader.segments[i];
        std::cout << "  [" << i << "] 0x" << std::hex
        << pseg->get_flags()
        << "\t0x"
        << pseg->get_virtual_address()
        << "\t0x"
        << pseg->get_file_size()
        << "\t0x"
        << pseg->get_memory_size()
        << std::endl;
        // Access to segments's data
        const char* p = reader.segments[i]->get_data();
        if (p != NULL) {
            uint32_t addr = (uint32_t)pseg->get_virtual_address();
            unsigned int count = 0;
            for (unsigned int i=0; i < pseg->get_memory_size(); i++, addr++) {
                if (count == 0) {
                    std::cout << std::hex << "0x" << addr << "\t0x" << (unsigned int)(p[i]&0x000000FF);
                }
                else {
                    std::cout << std::hex << " 0x" << (unsigned int)(p[i]&0x000000FF);
                }
                
                memory->storeByte(addr, p[i]);
                count++;
                if (count == 4) {
                    count = 0;
                    std::cout << std::endl;
                }
            }
        }
    }
    /*
    for ( int i = 0; i < sec_num; ++i ) {
        ELFIO::section* psec = reader.sections[i];
        // Check section type
        if ( psec->get_type() == SHT_SYMTAB ) {
            const ELFIO::symbol_section_accessor symbols( reader, psec );
            for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
                std::string   name;
                ELFIO::Elf64_Addr    value;
                ELFIO::Elf_Xword     size;
                unsigned char bind;
                unsigned char type;
                ELFIO::Elf_Half      section_index;
                unsigned char other;
                
                // Read symbol properties
                symbols.get_symbol( j, name, value, size, bind,
                                   type, section_index, other );
                std::cout << j << " " << name << " " << value << std::endl;
            }
        }
    }
     */
    
}