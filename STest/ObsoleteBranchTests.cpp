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
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x5043FFFD, cpu0->getControlCoprocessor());   // beql	v0,v1,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}

void MIPS_BGEZALL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x0453FFFD, cpu0->getControlCoprocessor());   // bgezall	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0x00400010u, cpu0->getRegister(31));
}

void MIPS_BGEZL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x0443FFFD, cpu0->getControlCoprocessor());   // bgezl	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}

void MIPS_BGTZL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x200200c8, cpu0->getControlCoprocessor());   // addi	v0,zero,200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x5C40FFFD, cpu0->getControlCoprocessor());   // bgtzl	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}

void MIPS_BLEZL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());   // addi	v0,zero,-200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x5840FFFD, cpu0->getControlCoprocessor());   // blezl	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}

void MIPS_BLTZALL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());   // addi	v0,zero,-200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x0452fffD, cpu0->getControlCoprocessor());   // bltzall	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
    ASSERT_EQUAL(0x00400010u, cpu0->getRegister(31));
}

void MIPS_BLTZL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());   // addi	v0,zero,-200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x0442FFFD, cpu0->getControlCoprocessor());   // bltzl	v0,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}

void MIPS_BNEL() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());   // addi	v0,zero,-200
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());   // addi	v1,zero,200
    memory->storeWord(0x00400008, 0x5443FFFD, cpu0->getControlCoprocessor());   // bnel	v0,v1,0x0
    memory->storeWord(0x0040000C, 0x00000000, cpu0->getControlCoprocessor());   // nop
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0x00400000u, cpu0->getPC()-4);
}
