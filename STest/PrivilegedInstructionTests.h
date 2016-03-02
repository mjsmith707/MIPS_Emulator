//
//  PrivilegedInstructionTests.hpp
//  MIPS_Emulator
//
//  Created by Matt on 3/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef PrivilegedInstructionTests_h
#define PrivilegedInstructionTests_h

#include "MIPS_Tests_Common.h"

void add_privileged_instruction_tests();

void MIPS_DI();
void MIPS_EI();
void MIPS_ERET();
void MIPS_MFC0();
void MIPS_MTC0();
void MIPS_RDPGPR();
void MIPS_TLBP();
void MIPS_TLBR();
void MIPS_TLBWI();
void MIPS_TLBWR();
void MIPS_WAIT();
void MIPS_WRPGPR();

#endif /* PrivilegedInstructionTests_h */
