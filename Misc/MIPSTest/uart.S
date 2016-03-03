//
//  uart.c
//  MIPSTest
//
//  Created by Matt on 3/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "uartdefs.h"

.text
.globl putch
.globl getch
.globl init_uart

init_uart:
    la $t0, UART_BASE       # load uart base address
    li $t1, UART_MCR_ON_1   # turn on LED that doesnt exist
    sb $t1, UART_MCR($t0)
    li $t1, UART_LCR_DLAB   # turn DLAB on
    sb $t1, UART_LCR($t0)
    li $t1, UART_DIVISOR
    sb $t1, UART_DLL($t0)   # write low order divisor byte
    srl $t1, $t1, 8
    sb $t1, UART_DLM($t0)   # write high order divisor byte
    li $t1, UART_LCR_W
    sb $t1, UART_LCR($t0)   # turn DLAB off
    jr $ra
    nop

putch:
    la $t0, UART_BASE       # load uart base address
putch0:
    lbu $t1, UART_LSR($t0)  # $t1 = line status register
    andi $t1, $t1, UART_LSR_TE # Check if transmitter is empty
    beqz $t1, putch0        # retry
    nop
    sb $a0, UART_DATA($t0)   # Send character
    jr $ra
    nop

getch:
    la $t0, UART_BASE       # load uart base address
getch0:
    lbu $t1, UART_LSR($t0)  # $t1 = line status register
    andi $t1, $t1, UART_LSR_DR  # check if data ready
    beqz $t1, getch0        # retry if not ready
    nop
    lbu $v0, UART_DATA($t0) # Get character
    jr $ra
    nop
