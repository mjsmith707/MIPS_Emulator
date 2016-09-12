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
#include "Device.h"
#include "UART8250.h"
#include "MC146818.h"
#include "SoftRes.h"
#include "ConsoleUI.h"
#include "FileLoader.h"
#include "elfio/elfio.hpp"

// All devices in the emulator
std::vector<Device*> devices;

// Initializes common devices
void initDevices(CPU* cpu0, PMMU* memory, ConsoleUI* consoleUI);

int main(int argc, const char * argv[]) {
    FileLoader loader;
    
    if (argc < 2) {
        std::cout << loader.usageInfo << std::endl;
        exit(0);
    }
    
    // Create memory management unit
    // Roughly 4GB
    PMMU* memory = new PMMU(1000000);
    
    // Create ConsoleUI
    ConsoleUI* consoleUI = new ConsoleUI();
    
    // Create cpu
    CPU* cpu0 = new CPU(0, consoleUI, memory);
    
    // Start paused
    cpu0->sendThreadSignal(CPU::PAUSE);
    
    // Initialize other devices
    initDevices(cpu0, memory, consoleUI);
    
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
    for (auto& dev : devices) {
        delete dev;
    }
    delete cpu0;
    delete memory;
    delete consoleUI;
    
    return 0;
}

// Initializes common devices
void initDevices(CPU* cpu0, PMMU* memory, ConsoleUI* consoleUI) {
    // Create uart8250
    UART8250* uart8250 = new UART8250();
    // Attach uart to iommu
    memory->attachDevice(uart8250);
    // Attach uart to consoleui
    consoleUI->attachUART(uart8250);
    devices.push_back(uart8250);
    
    // Create mc146818
    MC146818* mc146818 = new MC146818();
    // Attach cmos_rtc to iommu
    memory->attachDevice(mc146818);
    // Attach cmos_rtc to cpu
    mc146818->getCPUHandle(cpu0);
    cpu0->attachClockableDevice(mc146818);
    devices.push_back(mc146818);
    
    // Create SoftRes
    SoftRes* softres = new SoftRes();
    // Attach SoftRes to iommu
    memory->attachDevice(softres);
    devices.push_back(softres);
    
    // Attach cpu to ConsoleUI
    consoleUI->attachCPU(cpu0);
    // Attach memory to ConsoleUI
    consoleUI->attachMemory(memory);
    

}
