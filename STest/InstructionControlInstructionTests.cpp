//
//  InstructionControlInstructionTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "InstructionControlInstructionTests.h"
using namespace STest;

void add_instruction_control_instructions_tests() {
    add_test("CPU Instruction Control Instruction Tests", "MIPS_EHB", &MIPS_EHB);
    add_test("CPU Instruction Control Instruction Tests", "MIPS_PAUSE", &MIPS_PAUSE);
    add_test("CPU Instruction Control Instruction Tests", "MIPS_SSNOP", &MIPS_SSNOP);
}

void MIPS_EHB() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_PAUSE() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SSNOP() {
    TEST_NOT_IMPLEMENTED();
}