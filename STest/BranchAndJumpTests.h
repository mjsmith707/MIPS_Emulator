//
//  BranchAndJumpTests.h
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef BranchAndJumpTests_h
#define BranchAndJumpTests_h

#include "MIPS_Tests_Common.h"

void add_branch_and_jump_tests();

void MIPS_BEQ();
void MIPS_BGEZ();
void MIPS_BGEZAL();
void MIPS_BGTZ();
void MIPS_BLEZ();
void MIPS_BLTZ();
void MIPS_BLTZAL();
void MIPS_BNE();
void MIPS_J();
void MIPS_JAL();
void MIPS_JALR();
void MIPS_JALR_HB();
void MIPS_JALX();
void MIPS_JR();
void MIPS_JR_HB();

#endif /* BranchAndJumpTests_h */
