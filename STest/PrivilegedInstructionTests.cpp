//
//  PrivilegedInstructionTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 3/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "PrivilegedInstructionTests.h"
using namespace STest;

void add_privileged_instruction_tests() {
    add_test("CPU Privileged Instruction Tests", "MIPS_DI", &MIPS_DI);
    add_test("CPU Privileged Instruction Tests", "MIPS_EI", &MIPS_EI);
    add_test("CPU Privileged Instruction Tests", "MIPS_ERET", &MIPS_ERET);
    add_test("CPU Privileged Instruction Tests", "MIPS_MFC0", &MIPS_MFC0);
    add_test("CPU Privileged Instruction Tests", "MIPS_MTC0", &MIPS_MTC0);
    add_test("CPU Privileged Instruction Tests", "MIPS_RDPGPR", &MIPS_RDPGPR);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBP", &MIPS_TLBP);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBR", &MIPS_TLBR);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBWI", &MIPS_TLBWI);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBWR", &MIPS_TLBWR);
    add_test("CPU Privileged Instruction Tests", "MIPS_WAIT", &MIPS_WAIT);
    add_test("CPU Privileged Instruction Tests", "MIPS_WRPGPR", &MIPS_WRPGPR);
}

void MIPS_DI() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0); // $t0 = 0
    // Enable Interrupts
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) | STATUS_IE);
    uint32_t status = cop0->getRegister(CO0_STATUS);
    memory->storeWordPhys(0x00400000, 0x41686000); // di $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_IE);
    ASSERT_EQUAL(status, cpu0->getRegister(8));
}

void MIPS_EI() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0); // $t0 = 0
    // Disable Interrupts
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_IE));
    uint32_t status = cop0->getRegister(CO0_STATUS);
    memory->storeWordPhys(0x00400000, 0x41686020); // ei $t0
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_IE);
    ASSERT_EQUAL(status, cpu0->getRegister(8));
}

void MIPS_ERET() {
    // Status_erl = 1
    reset();
    cpu0->setPC(0x00400000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set Status_erl = 1
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) | STATUS_ERL);
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0xDEADBEE0);
    memory->storeWordPhys(0x00400000, 0x42000018); // eret
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEE0u, cpu0->getPC());
    
    // Status_erl = 0
    reset();
    cpu0->setPC(0x00400000);
    cop0 = cpu0->getControlCoprocessor();
    // Set Status_erl = 0
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_ERL));
    // Set EPC
    cop0->setRegisterHW(CO0_EPC, 0xDEADBEE0);
    memory->storeWordPhys(0x00400000, 0x42000018); // eret
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEE0u, cpu0->getPC());
}

// More thorough coprocessor 0 register testing will be done elsewhere
void MIPS_MFC0() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0); // $t0 = 0
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0xDEADBEEF);
    memory->storeWordPhys(0x00400000, 0x4008f000); // mfc0 $t0, c0_errorepc
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEEFu, cpu0->getRegister(8));
}

// More thorough coprocessor 0 register testing will be done elsewhere
void MIPS_MTC0() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xDEADBEEF); // $t0 = 0xDEADBEEF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0);
    memory->storeWordPhys(0x00400000, 0x4088f000); // mtc0 $t0, c0_errorepc
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEEFu, cop0->getRegister(CO0_ERROREPC));
}

void MIPS_RDPGPR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLBP() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLBR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLBWI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLBWR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_WAIT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_WRPGPR() {
    TEST_NOT_IMPLEMENTED();
}