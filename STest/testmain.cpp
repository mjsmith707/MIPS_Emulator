//
//  testmain.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/28/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "STest.h"
#include "MIPS_Tests_Common.h"
#include "ArithmeticTests.h"
#include "BranchAndJumpTests.h"
#include "InstructionControlInstructionTests.h"
#include "LoadStoreMemoryControlTests.h"
#include "LogicalTests.h"
#include "InsertExtractTests.h"
#include "MoveTests.h"
#include "ShiftTests.h"
#include "TrapTests.h"
#include "ObsoleteBranchTests.h"
#include "PrivilegedInstructionTests.h"
#include "ExceptionTests.h"

using namespace STest;

int main() {
    init_tests_common();
    add_arithmetic_tests();
    add_branch_and_jump_tests();
    add_instruction_control_instructions_tests();
    add_load_store_memory_control_tests();
    add_logical_tests();
    add_insert_extract_tests();
    add_move_tests();
    add_shift_tests();
    add_trap_tests();
    add_obsolete_branch_tests();
    add_privileged_instruction_tests();
    add_exception_tests();
    
    run_tests();
}