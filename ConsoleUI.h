//
//  ConsoleUI.h
//  MIPS_Emulator
//
//  Created by Matt on 2/5/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef ConsoleUI_h
#define ConsoleUI_h

#include <thread>
#include <curses.h>
#include <cstdlib>
#include "SharedQueue.h"
#include "UART8250.h"
#include "PMMU.h"

class CPU;  // Forward reference

// Handles stdin/stdout reading/writing
// Todo: Split waitForInput() into separate functions
// Make ncurses behave much nicer (i.e. better terminal emulation, scrolling etc.)
// Separate thread from main (if ncurses allows)
// Separate ncurses windows for uart, debug messages, etc.
class ConsoleUI {
    private:
        // CPU Pointer
        CPU* cpu;
    
        // Memory Pointer
        PMMU* memory;
    
        // Serial UART pointer
        UART8250* uart;
    
        // Lock protected queue for sending messages for logging etc.
        Shared_Queue<std::string> msgQueue;
    
        // Breakpoint target (if any)
        bool breakpointActive;
        bool singleStepActive;
        uint32_t breakpointAddr;
    
        // Helper functions
        void checkScrollBounds();
        void checkMessages();
        bool commandMenu();
    
    public:
        // CTOR
        ConsoleUI();
    
        // Attaches UART to ConsoleUI
        void attachUART(UART8250*);
    
        // Attaches CPU to ConsoleUI
        void attachCPU(CPU*);
    
        // Attaches Memory to ConsoleUI
        void attachMemory(PMMU*);
    
        // Polling IO loop
        void waitForInput();
    
        // Public function for sending a message to output
        void sendConsoleMsg(const std::string&);
};

#endif /* ConsoleUI_h */
