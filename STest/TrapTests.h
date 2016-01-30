//
//  TrapTests.h
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef TrapTests_h
#define TrapTests_h

#include "MIPS_Tests_Common.h"

void add_trap_tests();

void MIPS_BREAK();
void MIPS_SYSCALL();
void MIPS_TEQ();
void MIPS_TEQI();
void MIPS_TGE();
void MIPS_TGEI();
void MIPS_TGEIU();
void MIPS_TGEU();
void MIPS_TLT();
void MIPS_TLTI();
void MIPS_TLTIU();
void MIPS_TLTU();
void MIPS_TNE();
void MIPS_TNEI();

#endif /* TrapTests_h */
