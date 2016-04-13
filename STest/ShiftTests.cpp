//
//  ShiftTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ShiftTests.h"
using namespace STest;

void add_shift_tests() {
    add_test("CPU Shift Tests", "MIPS_ROTR", &MIPS_ROTR);
    add_test("CPU Shift Tests", "MIPS_ROTRV", &MIPS_ROTRV);
    add_test("CPU Shift Tests", "MIPS_SLL", &MIPS_SLL);
    add_test("CPU Shift Tests", "MIPS_SLLV", &MIPS_SLLV);
    add_test("CPU Shift Tests", "MIPS_SRA", &MIPS_SRA);
    add_test("CPU Shift Tests", "MIPS_SRAV", &MIPS_SRAV);
    add_test("CPU Shift Tests", "MIPS_SRL", &MIPS_SRL);
    add_test("CPU Shift Tests", "MIPS_SRLV", &MIPS_SRLV);
}

void MIPS_ROTR() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0);           // t1 = 0
    cpu0->setRegister(10, 0);           // t2 = 0
    memory->storeWordPhys(0x00400000, 0x00284802);   // rotr $t1, $t0, 32
    memory->storeWordPhys(0x00400004, 0x00285202);   // rotr $t2, $t0, 8
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(9));
    ASSERT_EQUAL(0xefdeadbeu, cpu0->getRegister(10));
}

void MIPS_ROTRV() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 64);           // t1 = 64
    cpu0->setRegister(10, 8);           // t2 = 8
    cpu0->setRegister(11, 0);           // t3 = 0
    cpu0->setRegister(12, 0);           // t4 = 0
    memory->storeWordPhys(0x00400000, 0x01285846);   // rotrv $t3, $t0, $t1   0xdeadbeef rotr 64
    memory->storeWordPhys(0x00400004, 0x01486046);   // rotrv $t4, $t0, $t2   0xdeadbeef rotr 8
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(11));
    ASSERT_EQUAL(0xefdeadbeu, cpu0->getRegister(12));
}

void MIPS_SLL() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0);            // t1 = 0
    cpu0->setRegister(10, 0);           // t2 = 0
    memory->storeWordPhys(0x00400000, 0x00084fc0);   // sll $t1, $t0, 31
    memory->storeWordPhys(0x00400004, 0x00085200);   // sll $t2, $t0, 8
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x80000000u, cpu0->getRegister(9));
    ASSERT_EQUAL(0xadbeef00u, cpu0->getRegister(10));
}

void MIPS_SLLV() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 31);           // t1 = 31
    cpu0->setRegister(10, 8);           // t2 = 8
    cpu0->setRegister(11, 0);           // t3 = 0
    cpu0->setRegister(12, 0);           // t4 = 0
    memory->storeWordPhys(0x00400000, 0x01285804);   // sllv $t3, $t0, $t1
    memory->storeWordPhys(0x00400004, 0x01486004);   // sllv $t4, $t0, $t2
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x80000000u, cpu0->getRegister(11));
    ASSERT_EQUAL(0xadbeef00u, cpu0->getRegister(12));
}

void MIPS_SRA() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0);            // t1 = 0
    cpu0->setRegister(10, 0);           // t2 = 0
    memory->storeWordPhys(0x00400000, 0x00084fc3);   // sra $t1, $t0, 31
    memory->storeWordPhys(0x00400004, 0x00085203);   // sra $t2, $t0, 8
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xffffffffu, cpu0->getRegister(9));
    ASSERT_EQUAL(0xffdeadbeu, cpu0->getRegister(10));
}

void MIPS_SRAV() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 31);           // t1 = 31
    cpu0->setRegister(10, 8);           // t2 = 8
    cpu0->setRegister(11, 0);           // t3 = 0
    cpu0->setRegister(12, 0);           // t4 = 0
    memory->storeWordPhys(0x00400000, 0x01285807);   // srav $t3, $t0, $t1
    memory->storeWordPhys(0x00400004, 0x01486007);   // srav $t4, $t0, $t2
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xffffffff, cpu0->getRegister(11));
    ASSERT_EQUAL(0xffdeadbeu, cpu0->getRegister(12));
}

void MIPS_SRL() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0);            // t1 = 0
    cpu0->setRegister(10, 0);           // t2 = 0
    memory->storeWordPhys(0x00400000, 0x00084fc2);   // srl $t1, $t0, 31
    memory->storeWordPhys(0x00400004, 0x00085202);   // srl $t2, $t0, 8
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x00000001u, cpu0->getRegister(9));
    ASSERT_EQUAL(0x00deadbeu, cpu0->getRegister(10));
}

void MIPS_SRLV() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 31);           // t1 = 31
    cpu0->setRegister(10, 8);           // t2 = 8
    cpu0->setRegister(11, 0);           // t3 = 0
    cpu0->setRegister(12, 0);           // t4 = 0
    memory->storeWordPhys(0x00400000, 0x01285806);   // srav $t3, $t0, $t1
    memory->storeWordPhys(0x00400004, 0x01486006);   // srav $t4, $t0, $t2
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x00000001u, cpu0->getRegister(11));
    ASSERT_EQUAL(0x00deadbeu, cpu0->getRegister(12));
}