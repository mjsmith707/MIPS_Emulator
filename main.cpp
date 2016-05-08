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
#include "FileLoader.h"
#include "elfio/elfio.hpp"

int main(int argc, const char * argv[]) {
    FileLoader loader;
    
    if (argc < 2) {
        std::cout << loader.usageInfo << std::endl;
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
    
    // Parse command line and load files
    loader.parseArguments(argc, argv, consoleUI, cpu0, memory);
    
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