//
//  TrapTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "TrapTests.h"
using namespace STest;

void add_trap_tests() {
    add_test("CPU Trap Tests", "MIPS_BREAK", &MIPS_BREAK);
    add_test("CPU Trap Tests", "MIPS_SYSCALL", &MIPS_SYSCALL);
    add_test("CPU Trap Tests", "MIPS_TEQ", &MIPS_TEQ);
    add_test("CPU Trap Tests", "MIPS_TEQI", &MIPS_TEQI);
    add_test("CPU Trap Tests", "MIPS_TGE", &MIPS_TGE);
    add_test("CPU Trap Tests", "MIPS_TGEI", &MIPS_TGEI);
    add_test("CPU Trap Tests", "MIPS_TGEIU", &MIPS_TGEIU);
    add_test("CPU Trap Tests", "MIPS_TGEU", &MIPS_TGEU);
    add_test("CPU Trap Tests", "MIPS_TLT", &MIPS_TLT);
    add_test("CPU Trap Tests", "MIPS_TLTI", &MIPS_TLTI);
    add_test("CPU Trap Tests", "MIPS_TLTIU", &MIPS_TLTIU);
    add_test("CPU Trap Tests", "MIPS_TLTU", &MIPS_TLTU);
    add_test("CPU Trap Tests", "MIPS_TNE", &MIPS_TNE);
    add_test("CPU Trap Tests", "MIPS_TNEI", &MIPS_TNEI);
}

void MIPS_BREAK() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SYSCALL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TEQ() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0);  // $t0 = 0x0
    cpu0->setRegister(9, 0x1);  // $t1 = 0x1
    cpu0->setRegister(10, 0x1); // $t2 = 0x1
    memory->storeWord(0x00400000, 0x01090034, cpu0->getControlCoprocessor()); // teq $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0034, cpu0->getControlCoprocessor()); // teq $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0);  // $t0 = 0x0
    cpu0->setRegister(9, 0x1);  // $t1 = 0x1
    cpu0->setRegister(10, 0x1); // $t2 = 0x1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_BEV)); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090034, cpu0->getControlCoprocessor()); // teq $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0034, cpu0->getControlCoprocessor()); // teq $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TEQI() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0000FFFF); // $t0 = 0x0000FFFF
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    memory->storeWord(0x00400000, 0x050cffff, cpu0->getControlCoprocessor()); // teqi $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052cffff, cpu0->getControlCoprocessor()); // teqi $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0000FFFF); // $t0 = 0x0000FFFF
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_BEV)); // Status_bev = 0
    memory->storeWord(0x00400000, 0x050cffff, cpu0->getControlCoprocessor()); // teqi $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052cffff, cpu0->getControlCoprocessor()); // teqi $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TGE() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEIU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTIU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TNE() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TNEI() {
    TEST_NOT_IMPLEMENTED();
}