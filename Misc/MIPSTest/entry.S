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
    di                  # Disable interrupts
    move $sp, $zero
    lui $sp, 0xAFFF 	# init stack pointer
    ori $sp, 0x0000
    lui $t0, 0x0100
    ori $t0, 0x0000
    mtc0 $t0, $11, 0    # Compare = 0x01000000
    lui $t0, 0x8000
    ori $t0, 0x0000
    mtc0 $t0, $15, 1    # Ebase = 0x80000000

    jal init_uart       # init uart
    nop

    mfc0 $t0, $12, 0    # Load Status
    lui $t1, 0xFFFF
    ori $t1, 0xFFF9
    and $t0, $t0, $t1   # Clear Status_erl,exl
    lui $t1, 0x0000
    ori $t1, 0x8000
    or $t0, $t0, $t1    # Set Status_im7
    mtc0 $t0, $12, 0	# Save Status
    mtc0 $zero, $9, 0	# Count = 0
    ei                  # Enable interrupts

    jal main            # call main
    nop
