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
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x3c02dead, cpu0->getControlCoprocessor());   // lui v0,0xdead
    memory->storeWord(0x00000004, 0x3c030001, cpu0->getControlCoprocessor());   // lui v1,0x1
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xdead0000u, cpu0->getRegister(2));
    ASSERT_EQUAL(0x00010000u, cpu0->getRegister(3));
}

void MIPS_NOR() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x00432027, cpu0->getControlCoprocessor());   // nor a0,v0,v1
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0xfffffe03u, cpu0->getRegister(4));
}

void MIPS_OR() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x00432025, cpu0->getControlCoprocessor());   // or a0,v0,v1
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0x000001fcu, cpu0->getRegister(4));
}

void MIPS_ORI() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x3444dead, cpu0->getControlCoprocessor());   // ori a0,v0,0xdead
    memory->storeWord(0x0000000C, 0x346500fe, cpu0->getControlCoprocessor());   // ori a1,v1,0xfe
    cpu0->stepCPU(4);
    ASSERT_EQUAL(0x0000deedu, cpu0->getRegister(4));
    ASSERT_EQUAL(0x000001feu, cpu0->getRegister(5));
}

void MIPS_XOR() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x00432026, cpu0->getControlCoprocessor());   // xor a0,v0,v1
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0x0000013cu, cpu0->getRegister(4));
}

void MIPS_XORI() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());   // addi	v1,zero,500
    memory->storeWord(0x00000008, 0x3844dead, cpu0->getControlCoprocessor());   // xori a0,v0,0xdead
    memory->storeWord(0x0000000C, 0x386500fe, cpu0->getControlCoprocessor());   // xori a1,v1,0xfe
    cpu0->stepCPU(4);
    ASSERT_EQUAL(0x0000de65u, cpu0->getRegister(4));
    ASSERT_EQUAL(0x0000010au, cpu0->getRegister(5));
}