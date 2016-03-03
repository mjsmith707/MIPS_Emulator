//
//  main.c
//  MIPSTest
//
//  Created by Matt on 3/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "uart.h"

// Last line read from UART
char line[256];
char msg[] = "Hey buddy I read a line from you!\nYou Said: ";
char newline[] = "\n";
int main(void) {
    //putline(msg);
    //putline(msg);
    for (;;) {
        getline(line);
        putline(msg);
        putline(line);
        putline(newline);
    }
    return 0;
}
