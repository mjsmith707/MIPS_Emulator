//
//  ConsoleUI.cpp
//  MIPS_Emulator
//
//  Created by Matt on 2/5/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ConsoleUI.h"

ConsoleUI::ConsoleUI(UART8250* uartptr) : uart(uartptr) {
    
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
    
    // Blocking input/output loop
    while (ch != '~') {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
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
            else {
                uart->sendChar(ch);
            }
        }
        else {
            ch = 0;
        }
        
        // Check if message is available from queue
        while (!msgQueue.isEmpty()) {
            // This will need to be split into a new window at some point but for now it will have to do
            std::string msg = msgQueue.pop();
            printw("LOM> %s\n", msg.c_str());
            refresh();
        }
    }
    
    // Check if message is available from queue
    while (!msgQueue.isEmpty()) {
        // This will need to be split into a new window at some point but for now it will have to do
        std::string msg = msgQueue.pop();
        printw("LOM> %s\n", msg.c_str());
        refresh();
    }
    
    // Todo: This is going to have to signal CPUs and stuff to exit
    // So we can get nice things like the last fetch-decode-execute output
    printw("Press any key to exit.");
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

// Public function for sending messages to the console
void ConsoleUI::sendConsoleMsg(const std::string& msg) {
    msgQueue.push(msg);
}