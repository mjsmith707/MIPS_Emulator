//
//  entry.s
//  MIPSTest
//
//  Created by Matt on 3/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//


.text

.globl _start
_start:
    nop
    move $sp, $zero
    lui $sp, 0x4FF4 # init stack pointer
    jal init_uart # init uart
    nop
    jal main # call main
    nop
