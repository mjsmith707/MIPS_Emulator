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
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    memory->storeWord(0x00400000, 0x0000000d, cpu0->getControlCoprocessor()); // break
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x0000000d, cpu0->getControlCoprocessor()); // break
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_SYSCALL() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x0000000c, cpu0->getControlCoprocessor()); // syscall
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x0000000c, cpu0->getControlCoprocessor()); // syscall
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
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
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0);  // $t0 = 0x0
    cpu0->setRegister(9, 0x1);  // $t1 = 0x1
    cpu0->setRegister(10, 0x1); // $t2 = 0x1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
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
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x050cffff, cpu0->getControlCoprocessor()); // teqi $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052cffff, cpu0->getControlCoprocessor()); // teqi $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TGE() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0);  // $t0 = 0
    cpu0->setRegister(9, 1);    // $t1 = 1
    cpu0->setRegister(10, -1);  // $t2 = -1
    memory->storeWord(0x00400000, 0x01090030, cpu0->getControlCoprocessor()); // tge $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0030, cpu0->getControlCoprocessor()); // tge $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0);  // $t0 = 0
    cpu0->setRegister(9, 1);  // $t1 = 1
    cpu0->setRegister(10, -1); // $t2 = -1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090030, cpu0->getControlCoprocessor()); // tge $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0030, cpu0->getControlCoprocessor()); // tge $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TGEI() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFF1); // $t0 = 0xFFFFFFF1
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    memory->storeWord(0x00400000, 0x0508ffff, cpu0->getControlCoprocessor()); // tgei $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x0528ffff, cpu0->getControlCoprocessor()); // tgei $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFF1); // $t0 = 0xFFFFFFF1
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x0508ffff, cpu0->getControlCoprocessor()); // tgei $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x0528ffff, cpu0->getControlCoprocessor()); // tgei $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TGEIU() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0FFFFFFF); // $t0 = 0x0FFFFFFF
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    memory->storeWord(0x00400000, 0x0509ffff, cpu0->getControlCoprocessor()); // tgeiu $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x0529ffff, cpu0->getControlCoprocessor()); // tgeiu $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0x0FFFFFFF); // $t0 = 0x0FFFFFFF
    cpu0->setRegister(9, 0xFFFFFFFF); // $t1 = 0xFFFFFFFF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x0509ffff, cpu0->getControlCoprocessor()); // tgeiu $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x0529ffff, cpu0->getControlCoprocessor()); // tgeiu $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TGEU() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0);  // $t0 = 0
    cpu0->setRegister(9, 2);  // $t1 = 2
    cpu0->setRegister(10, 1); // $t2 = 1
    memory->storeWord(0x00400000, 0x01090031, cpu0->getControlCoprocessor()); // tgeu $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0031, cpu0->getControlCoprocessor()); // tgeu $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0);    // $t0 = 0
    cpu0->setRegister(9, 2);    // $t1 = 2
    cpu0->setRegister(10, 1);   // $t2 = 1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090031, cpu0->getControlCoprocessor()); // tgeu $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0031, cpu0->getControlCoprocessor()); // tgeu $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TLT() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);    // $t0 = 1
    cpu0->setRegister(9, -1);   // $t1 = -1
    cpu0->setRegister(10, 0);   // $t2 = 0
    memory->storeWord(0x00400000, 0x01090032, cpu0->getControlCoprocessor()); // tlt $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0032, cpu0->getControlCoprocessor()); // tlt $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);    // $t0 = 1
    cpu0->setRegister(9, -1);   // $t1 = -1
    cpu0->setRegister(10, 0);   // $t2 = 0
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090032, cpu0->getControlCoprocessor()); // tlt $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0032, cpu0->getControlCoprocessor()); // tlt $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TLTI() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0xFFFFFFF1); // $t1 = 0xFFFFFFF1
    memory->storeWord(0x00400000, 0x050affff, cpu0->getControlCoprocessor()); // tlti $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052affff, cpu0->getControlCoprocessor()); // tlti $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0xFFFFFFF1); // $t1 = 0xFFFFFFF1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x050affff, cpu0->getControlCoprocessor()); // tlti $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052affff, cpu0->getControlCoprocessor()); // tlti $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TLTIU() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0xFFFFFFF1); // $t1 = 0xFFFFFFF1
    memory->storeWord(0x00400000, 0x050bffff, cpu0->getControlCoprocessor()); // tltiu $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052bffff, cpu0->getControlCoprocessor()); // tltiu $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0xFFFFFFF1); // $t1 = 0xFFFFFFF1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x050bffff, cpu0->getControlCoprocessor()); // tltiu $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052bffff, cpu0->getControlCoprocessor()); // tltiu $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}

void MIPS_TLTU() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);  // $t0 = 1
    cpu0->setRegister(9, 0);  // $t1 = 0
    cpu0->setRegister(10, 2); // $t2 = 2
    memory->storeWord(0x00400000, 0x01090033, cpu0->getControlCoprocessor()); // tltu $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0033, cpu0->getControlCoprocessor()); // tltu $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);  // $t0 = 1
    cpu0->setRegister(9, 0);  // $t1 = 0
    cpu0->setRegister(10, 2); // $t2 = 2
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090033, cpu0->getControlCoprocessor()); // tltu $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0033, cpu0->getControlCoprocessor()); // tltu $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TNE() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);    // $t0 = 1
    cpu0->setRegister(9, 1);    // $t1 = 1
    cpu0->setRegister(10, -1);  // $t2 = -1
    memory->storeWord(0x00400000, 0x01090036, cpu0->getControlCoprocessor()); // tne $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0036, cpu0->getControlCoprocessor()); // tne $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 1);    // $t0 = 1
    cpu0->setRegister(9, 1);    // $t1 = 1
    cpu0->setRegister(10, -1);  // $t2 = -1
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x01090036, cpu0->getControlCoprocessor()); // tne $t0, $t1 should skip
    memory->storeWord(0x00400004, 0x012a0036, cpu0->getControlCoprocessor()); // tne $t1, $t2 should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());   // Status_bev = 0 vector
}

void MIPS_TNEI() {
    // Status_bev = 1 (Default), 0xBFC0.0380
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0x0FFFFFFF); // $t1 = 0x0FFFFFFF
    memory->storeWord(0x00400000, 0x050effff, cpu0->getControlCoprocessor()); // tnei $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052effff, cpu0->getControlCoprocessor()); // tnei $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xBFC00380u, cpu0->getPC());    // Status_bev = 1 vector
    
    // Status_bev = 0, 0x8000.0180
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xFFFFFFFF); // $t0 = 0xFFFFFFFF
    cpu0->setRegister(9, 0x0FFFFFFF); // $t1 = 0x0FFFFFFF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV); // Status_bev = 0
    memory->storeWord(0x00400000, 0x050effff, cpu0->getControlCoprocessor()); // tnei $t0, 0xFFFF should skip
    memory->storeWord(0x00400004, 0x052effff, cpu0->getControlCoprocessor()); // tnei $t1, 0xFFFF should trap
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00400004u, cpu0->getPC());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());    // Status_bev = 0 vector
}