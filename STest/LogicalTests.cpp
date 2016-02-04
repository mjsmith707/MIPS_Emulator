//
//  LogicalTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "LogicalTests.h"
using namespace STest;

void add_logical_tests() {
    add_test("CPU Logical Tests", "MIPS_AND", &MIPS_AND);
    add_test("CPU Logical Tests", "MIPS_ANDI", &MIPS_ANDI);
    add_test("CPU Logical Tests", "MIPS_LUI", &MIPS_LUI);
    add_test("CPU Logical Tests", "MIPS_NOR", &MIPS_NOR);
    add_test("CPU Logical Tests", "MIPS_OR", &MIPS_OR);
    add_test("CPU Logical Tests", "MIPS_ORI", &MIPS_ORI);
    add_test("CPU Logical Tests", "MIPS_XOR", &MIPS_XOR);
    add_test("CPU Logical Tests", "MIPS_XORI", &MIPS_XORI);
}

void MIPS_AND() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());   // addi	a0,zero,-200
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());   // addi	a1,zero,-500
    memory->storeWord(0x00000010, 0x00434024, cpu0->getControlCoprocessor());   // and	t0,v0,v1
    memory->storeWord(0x00000014, 0x00454824, cpu0->getControlCoprocessor());   // and	t1,v0,a1
    memory->storeWord(0x00000018, 0x00835024, cpu0->getControlCoprocessor());   // and	t2,a0,v1
    memory->storeWord(0x0000001C, 0x00855824, cpu0->getControlCoprocessor());   // and	t3,a0,a1
    cpu0->stepCPU(8);
    ASSERT_EQUAL(192u, cpu0->getRegister(8));
    ASSERT_EQUAL(8u, cpu0->getRegister(9));
    ASSERT_EQUAL(304u, cpu0->getRegister(10));
    ASSERT_EQUAL(0xfffffe08u, cpu0->getRegister(11));
}

void MIPS_ANDI() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());   // addi	a0,zero,-200
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());   // addi	a1,zero,-500
    memory->storeWord(0x00000010, 0x304801f4, cpu0->getControlCoprocessor());   // andi	t0,v0,0x1f4
    memory->storeWord(0x00000014, 0x3049fe0c, cpu0->getControlCoprocessor());   // andi	t1,v0,0xfe0c
    memory->storeWord(0x00000018, 0x308a01f4, cpu0->getControlCoprocessor());   // andi	t2,a0,0x1f4
    memory->storeWord(0x0000001C, 0x308bfe0c, cpu0->getControlCoprocessor());   // andi	t3,a0,0xfe0c
    cpu0->stepCPU(8);
    ASSERT_EQUAL(192u, cpu0->getRegister(8));
    ASSERT_EQUAL(8u, cpu0->getRegister(9));
    ASSERT_EQUAL(304u, cpu0->getRegister(10));
    ASSERT_EQUAL(65032u, cpu0->getRegister(11));
}

void MIPS_LUI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_NOR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_OR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_ORI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_XOR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_XORI() {
    TEST_NOT_IMPLEMENTED();
}