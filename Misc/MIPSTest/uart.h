#ifndef _UART_h
#define _UART_h

#include "uartdefs.h"

// Sends a character to the UART
extern void putch(char);

// Gets a character from the UART
extern char getch();

// Gets a line from the UART
// Returns number of characters read
int getline(char*);

// Sends a string to the UART
void putline(char*);

#endif
