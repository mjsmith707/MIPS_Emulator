//
//  reset.S
//  MIPSTest
//
//  Created by Matt on 3/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

.data
# Storage for saving context
sregs: .word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
intmsg: .asciiz "Interrupt!\n"

# From pg 76 vol3
.text
.globl reset_exception
.set noat
reset_exception:
	j _start
	nop
	
.align 10
IVexception:
#    mfc0 $k0, CO0_CAUSE
#    mfc0 $k1, CO0_STATUS
#    andi $k0, $k0, CAUSE_IM
#    and $k0, $k0, $k1
#    beq $k0, $zero,
#    clz $k0, $k0
#    xori $k0, $k0, 0x17
#    sll $k0, $k0, VS
#    la $k1, VectorBase
#    addu $k0, $k0, $k1
#    jr $k0
#    nop

# Save registers
SaveRegsFull:
	la $k0, sregs
	sw $at, 0($k0)
	sw $v0, 4($k0)
	sw $v1, 8($k0)
	sw $a0, 12($k0)
	sw $a2, 16($k0)
	sw $a3, 20($k0)
	sw $t0, 24($k0)
	sw $t1, 28($k0)
	sw $t2, 32($k0)
	sw $t3, 36($k0)
	sw $t4, 40($k0)
	sw $t5, 44($k0)
	sw $t6, 48($k0)
	sw $t7, 52($k0)
	sw $t8, 56($k0)
	sw $gp, 60($k0)
	sw $sp, 64($k0)
	sw $ra, 68($k0)	

# Prove that an interrupt was generated
writemsg:
    move $sp, $zero
    lui $sp, 0xBF00 	# init kernel stack pointer
    la $a0, intmsg      # Load &intmsg
    jal putline         # call putline(intmsg)
    nop

# Restore registers
RestoreRegsFull:
	la $k0, sregs
	lw $at, 0($k0)
	lw $v0, 4($k0)
	lw $v1, 8($k0)
	lw $a0, 12($k0)
	lw $a2, 16($k0)
	lw $a3, 20($k0)
	lw $t0, 24($k0)
	lw $t1, 28($k0)
	lw $t2, 32($k0)
	lw $t3, 36($k0)
	lw $t4, 40($k0)
	lw $t5, 44($k0)
	lw $t6, 48($k0)
	lw $t7, 52($k0)
	lw $t8, 56($k0)
	lw $gp, 60($k0)
	lw $sp, 64($k0)
	lw $ra, 68($k0)	

# Return from interrupt
intret:
    lui $k0, 0x0100
    ori $k0, 0x0000
    mtc0 $k0, $11, 0    # Clear Timer Compare = 0x01000000
    mtc0 $zero, $9, 0	# Count = 0
    ei                  # Enable Interrupts
    eret                # Exception Return
    nop
