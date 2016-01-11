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
#include <curses.h>
#include "PMMU.h"
#include "CPU.h"
#include "UART8250.h"
#include "elfio/elfio.hpp"

void loadFile(const char*, PMMU*, CPU*);
void waitForInput(UART8250* uart);

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        std::cout << "Usage ./MIPS_Emulator <elf_filename>" << std::endl;
        exit(0);
    }
    
    // Create memory management unit
    // Roughly 4GB
    PMMU* memory = new PMMU(1000000);
    
    // Create cpu
    CPU* cpu0 = new CPU(memory);
    
    // Create uart8250
    UART8250* uart8250 = new UART8250();
    
    // Attach uart to iommu
    memory->attachDevice(uart8250);
    
    // Load binary
    loadFile(argv[1], memory, cpu0);
    
    // Start CPU
    std::thread cpu0_thread(std::bind(&CPU::start, cpu0));
    //std::this_thread::sleep_for(std::chrono::seconds(60));
    
    // Wait for console input
    waitForInput(uart8250);
    
    // Stop CPU
    cpu0->sendSignal(1);
    cpu0_thread.join();
    return 0;
}

// Handles keyboard input/output to the vm
// Ncurses is like black magic
// http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
void waitForInput(UART8250* uart) {
    // Initialize ncurses
    char ch = 0;
    initscr();
    timeout(0);
    noecho();
    scrollok(stdscr, TRUE);
    idlok(stdscr, TRUE);
    
    // Blocking input/output loop
    while (ch != '~') {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        // Check if character is available from uart
        if (uart->getChar(&ch)) {
            // Ncurses hates newlines...
            if (ch == '\n') {
                // Check if we're at the bottom of the window
                // If so then scroll otherwise move the cursor
                // Just printing \n causes it to overwrite the line...
                int x, y;
                int xmax, ymax;
                getyx(stdscr, y, x);
                getmaxyx(stdscr, ymax, xmax);
                if (y+1 == ymax) {
                    scroll(stdscr);
                }
                else {
                    wmove(stdscr, y+1, x);
                }
            }
            else {
                printw("%c", ch);
                refresh();
            }
        }
        
        // Check if character is available from stdin
        ch = getch();
        if (ch > 0) {
            // For whatever reason enter comes in as LF
            // Send CR instead
            if (ch == '\n') {
                uart->sendChar(0xD);
            }
            else {
                uart->sendChar(ch);
            }
        }
        else {
            ch = 0;
        }
    }
    
    // Cleanup ncurses
    endwin();
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
        const char* p = pseg->get_data();
        if (p != NULL) {
            // Load segment into memory
            uint32_t addr = (uint32_t)pseg->get_virtual_address();
            for (uint32_t j=0; j < pseg->get_file_size(); j++, addr++) {
                memory->storeByte(addr, p[j]);
            }
        }
    }
}