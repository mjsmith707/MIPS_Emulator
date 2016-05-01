//
//  ObsoleteBranchTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/30/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ObsoleteBranchTests.h"
using namespace STest;

void add_obsolete_branch_tests() {
    add_test("CPU Obsolete Branch Tests", "MIPS_BEQL", &MIPS_BEQL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BGEZALL", &MIPS_BGEZALL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BGEZL", &MIPS_BGEZL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BGTZL", &MIPS_BGTZL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BLEZL", &MIPS_BLEZL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BLTZALL", &MIPS_BLTZALL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BLTZL", &MIPS_BLTZL);
    add_test("CPU Obsolete Branch Tests", "MIPS_BNEL", &MIPS_BNEL);
}

void MIPS_BEQL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x5043FFFD);   // beql	v0,v1,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BGEZALL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0453FFFD);   // bgezall	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0000010u, cpu0->getRegister(31));
}

void MIPS_BGEZL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0443FFFD);   // bgezl	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BGTZL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x5C40FFFD);   // bgtzl	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BLEZL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x5840FFFD);   // blezl	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BLTZALL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0452fffD);   // bltzall	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0xA0000010u, cpu0->getRegister(31));
}

void MIPS_BLTZL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x0442FFFD);   // bltzl	v0,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}

void MIPS_BNEL() {
    reset();
    cpu0->setPC(0xA0000000);
    memory->storeWordPhys(0xA0000000, 0x2002ff38);   // addi	v0,zero,-200
    memory->storeWordPhys(0xA0000004, 0x200300c8);   // addi	v1,zero,200
    memory->storeWordPhys(0xA0000008, 0x5443FFFD);   // bnel	v0,v1,0x0
    memory->storeWordPhys(0xA000000C, 0x00000000);   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xA0000000u, cpu0->getPC()-4);
}
