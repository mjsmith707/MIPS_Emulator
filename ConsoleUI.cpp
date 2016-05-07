//
//  ConsoleUI.cpp
//  MIPS_Emulator
//
//  Created by Matt on 2/5/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ConsoleUI.h"
#include "CPU.h"

ConsoleUI::ConsoleUI(UART8250* uartptr) : uart(uartptr), breakpointActive(false), breakpointAddr(0) {
    
}

// Handles keyboard input/output to the vm
// Ncurses is like black magic
// http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
void ConsoleUI::waitForInput() {
    // Initialize ncurses
    char ch = 0;
    initscr();
    timeout(0);
    noecho();
    scrollok(stdscr, TRUE);
    idlok(stdscr, TRUE);
    
    bool running = commandMenu();
    // Blocking input/output loop
    while (running) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(50));
        
        // Check if character is available from uart
        if (uart->getChar(&ch)) {
            // Ncurses hates newlines...
            if (ch == '\n') {
                checkScrollBounds();
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
            else if (ch == '~') {
                cpu->sendThreadSignal(CPU::PAUSE);
                running = commandMenu();
            }
            else {
                uart->sendChar(ch);
            }
        }
        else {
            ch = 0;
        }
        
        // Check if breakpoint is active and reached
        if (breakpointActive && (cpu->getThreadSignal() == CPU::PAUSE)) {
            printw("LOM> Breakpoint %#010x reached.", breakpointAddr);
            breakpointActive = false;
            if (singleStepActive) {
                printw("\n%s", cpu->debugPrint().c_str());
                singleStepActive = false;
            }
            refresh();
            running = commandMenu();
        }
        
        // Check if message is available from queue
        size_t count = 0;
        while ((count < 20) && (!msgQueue.isEmpty())) {
            // This will need to be split into a new window at some point but for now it will have to do
            std::string msg = msgQueue.pop();
            printw("LOM> %s\n", msg.c_str());
            refresh();
            count++;
        }
    }
    
    // Halt CPU
    cpu->sendThreadSignal(CPU::HALT);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Should be long enough for the thread to respond.
    
    // Check if message is available from queue
    while (!msgQueue.isEmpty()) {
        // This will need to be split into a new window at some point but for now it will have to do
        std::string msg = msgQueue.pop();
        printw("LOM> %s\n", msg.c_str());
        refresh();
    }
    
    // Leaving
    printw("Press any key to quit.");
    refresh();
    std::cin >> ch;
    
    // Cleanup ncurses
    endwin();
}

void ConsoleUI::checkScrollBounds() {
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

// Very simple command interface
// On entry, the CPU thread is paused, waiting for instructions
bool ConsoleUI::commandMenu() {
    bool retval = false;
    // Turn on canoical io
    echo();
    nocbreak();
    timeout(-1);
    
    char input[256] = {0};
    for (;;) {
        printw("\nLOM> ");
        refresh();
        getstr(input);
        refresh();
        switch (input[0]) {
            // Quit
            case 'q': {
                printw("\n");
                refresh();
                retval = false;
                breakpointActive = false;
                goto out;
            }
            // Continue execution
            case 'c': {
                printw("Continuing...\n");
                refresh();
                if (!breakpointActive) {
                    cpu->sendThreadSignal(CPU::CONTINUE);
                }
                retval = true;
                goto out;
            }
            // Debug Print
            case 'd': {
                printw("%s", cpu->debugPrint().c_str());
                refresh();
                break;
            }
            // Print GPRs
            case 'g': {
                printw("%s", cpu->printRegisters().c_str());
                refresh();
                break;
            }
            // Examine Memory
            case 'x': {
                printw("Enter address: ");
                refresh();
                getstr(input);
                refresh();
                uint32_t addr = strtoul(input, NULL, 0);
                uint32_t word = 0;
                for (uint32_t i=0; i<20; i++) {
                    if (i % 5 == 0) {
                        printw("\n%#010x ", addr);
                    }
                    memory->readWordPhys(addr, &word);
                    printw("%#010x ", word);
                    addr += 4;
                }
                printw("\n\n");
                break;
            }
            // Breakpoint
            case 'b': {
                printw("Enter address: ");
                refresh();
                getstr(input);
                refresh();
                breakpointAddr = strtoul(input, NULL, 0);
                breakpointActive = true;
                cpu->setBreakpoint(breakpointAddr);
                printw("Breakpoint %#010x set.\n", breakpointAddr);
                refresh();
                cpu->sendThreadSignal(CPU::BREAKPTACTIVE);
                break;
            }
            // Single step
            case 's': {
                breakpointActive = true;
                singleStepActive = true;
                cpu->sendThreadSignal(CPU::STEP);
                retval = true;
                goto out;
            }
            // Help
            case 'h':
            default: {
                const std::string help = "Command List:\nh = help\nq = quit\nc = continue\nd = debug print\ng = print registers\nx = examine memory\nb = set breakpoint\ns = single step";
                printw("%s", help.c_str());
                refresh();
                break;
            }
        }
    }

// Return to non-canonical mode
out:
    noecho();
    cbreak();
    timeout(0);
    return retval;
}

// Attaches CPU to ConsoleUI
void ConsoleUI::attachCPU(CPU* cpu) {
    if (cpu == nullptr) {
        throw std::runtime_error("Got null pointer in ConsoleUI::attachCPU");
    }
    this->cpu = cpu;
}

// Attaches CPU to ConsoleUI
void ConsoleUI::attachMemory(PMMU* memory) {
    if (memory == nullptr) {
        throw std::runtime_error("Got null pointer in ConsoleUI::attachMemory");
    }
    this->memory = memory;
}

// Public function for sending messages to the console
void ConsoleUI::sendConsoleMsg(const std::string& msg) {
    msgQueue.push(msg);
}