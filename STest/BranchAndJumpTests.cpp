//
//  BranchAndJumpTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "BranchAndJumpTests.h"
using namespace STest;

void add_branch_and_jump_tests() {
    add_test("CPU Branch and Jump Tests", "MIPS_BEQ", &MIPS_BEQ);
    add_test("CPU Branch and Jump Tests", "MIPS_BGEZ", &MIPS_BGEZ);
    add_test("CPU Branch and Jump Tests", "MIPS_BGEZAL", &MIPS_BGEZAL);
    add_test("CPU Branch and Jump Tests", "MIPS_BGTZ", &MIPS_BGTZ);
    add_test("CPU Branch and Jump Tests", "MIPS_BLEZ", &MIPS_BLEZ);
    add_test("CPU Branch and Jump Tests", "MIPS_BLTZ", &MIPS_BLTZ);
    add_test("CPU Branch and Jump Tests", "MIPS_BLTZAL", &MIPS_BLTZAL);
    add_test("CPU Branch and Jump Tests", "MIPS_BNE", &MIPS_BNE);
    add_test("CPU Branch and Jump Tests", "MIPS_J", &MIPS_J);
    add_test("CPU Branch and Jump Tests", "MIPS_JAL", &MIPS_JAL);
    add_test("CPU Branch and Jump Tests", "MIPS_JALR", &MIPS_JALR);
    add_test("CPU Branch and Jump Tests", "MIPS_JALR.HB", &MIPS_JALR_HB);
    add_test("CPU Branch and Jump Tests", "MIPS_JALX", &MIPS_JALX);
    add_test("CPU Branch and Jump Tests", "MIPS_JR", &MIPS_JR);
    add_test("CPU Branch and Jump Tests", "MIPS_JR.HB", &MIPS_JR_HB);
}

void MIPS_BEQ() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x1043fffd);   // beq	v0,v1,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BGEZ() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0441fffd);   // bgez	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BGEZAL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0451fffd);   // bgezal	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0000010u, cpu0->getRegister(31));
}

void MIPS_BGTZ() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x1c40fffd);   // bgtz	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BLEZ() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x1840fffd);   // blez	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BLTZ() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0440fffd);   // bltz	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BLTZAL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0450fffd);   // bltzal	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0000010u, cpu0->getRegister(31));
}

void MIPS_BNE() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x1443fffd);   // bne	v0,v1,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_J() {
    reset();
    cpu0->setPC(0xA0400000);
    memory->storeWordPhys(0xA0400000, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400004, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400008, 0x00000000);   // nop
    memory->storeWordPhys(0xA040000c, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400010, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400014, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400018, 0x00000000);   // nop
    memory->storeWordPhys(0xA040001c, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400020, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400024, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400028, 0x08100000);   // j	0xA0400000
    memory->storeWordPhys(0xA040002c, 0x00000000);   // nop
    cpu0->stepCPU(13);
    ASSERT_EQUAL(0xA0400000u, cpu0->getPC()-4);
}

void MIPS_JAL() {
    reset();
    cpu0->setPC(0xA0400000);
    memory->storeWordPhys(0xA0400000, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400004, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400008, 0x00000000);   // nop
    memory->storeWordPhys(0xA040000c, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400010, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400014, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400018, 0x00000000);   // nop
    memory->storeWordPhys(0xA040001c, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400020, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400024, 0x00000000);   // nop
    memory->storeWordPhys(0xA0400028, 0x0c100000);   // jal	0xA0400000
    memory->storeWordPhys(0xA040002c, 0x00000000);   // nop
    cpu0->stepCPU(13);
    ASSERT_EQUAL(0xA0400000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0400030u, cpu0->getRegister(31));
}

void MIPS_JALR() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ffcb);   // addi	v0,zero,-53
    memory->storeWordPhys(0xA0000004, 0x3c01A000);   // lui	at,0xA000
    memory->storeWordPhys(0xA0000008, 0x34240000);   // ori	a0,at,0x0000
    memory->storeWordPhys(0xA000000c, 0x0080f809);   // jalr	a0
    memory->storeWordPhys(0xA0000010, 0x00000000);   // nop
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0000014u, cpu0->getRegister(31));
}

void MIPS_JALR_HB() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_JALX() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_JR() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ffcb);   // addi	v0,zero,-53
    memory->storeWordPhys(0xA0000004, 0x3c01A000);   // lui	at,0xA000
    memory->storeWordPhys(0xA0000008, 0x34240000);   // ori	a0,at,0x0000
    memory->storeWordPhys(0xA000000c, 0x00800008);   // jr	a0
    memory->storeWordPhys(0xA0000010, 0x00000000);   // nop
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_JR_HB() {
    TEST_NOT_IMPLEMENTED();
}