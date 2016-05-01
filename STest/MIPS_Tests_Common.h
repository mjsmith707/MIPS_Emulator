//
//  MIPS_Tests_Common.h
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef MIPS_Tests_Common_h
#define MIPS_Tests_Common_h

// Common headers, functions and variables for cpu testing

#include "STest.h"
#include "PMMU.h"
#include "CPU.h"

extern PMMU* memory;
extern CPU* cpu0;

void init_tests_common();
void reset();

#endif /* MIPS_Tests_Common_h */
