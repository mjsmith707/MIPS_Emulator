//
//  MoveTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "MoveTests.h"
using namespace STest;

void add_move_tests() {
    add_test("CPU Move Tests", "MIPS_MFHI", &MIPS_MFHI);
    add_test("CPU Move Tests", "MIPS_MFLO", &MIPS_MFLO);
    add_test("CPU Move Tests", "MIPS_MOVF", &MIPS_MOVF);
    add_test("CPU Move Tests", "MIPS_MOVN", &MIPS_MOVN);
    add_test("CPU Move Tests", "MIPS_MOVT", &MIPS_MOVT);
    add_test("CPU Move Tests", "MIPS_MOVZ", &MIPS_MOVZ);
    add_test("CPU Move Tests", "MIPS_MTHI", &MIPS_MTHI);
    add_test("CPU Move Tests", "MIPS_MTLO", &MIPS_MTLO);
    add_test("CPU Move Tests", "MIPS_RDHWR", &MIPS_RDHWR);
}

// This starts the era of more explicit tests...
void MIPS_MFHI() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setHI(0xdeadbeef);
    cpu0->setRegister(8, 0x0);
    memory->storeWordPhys(0x00400000, 0x00004010);   // mfhi $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
}

void MIPS_MFLO() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setLO(0xdeadbeef);
    cpu0->setRegister(8, 0x0);
    memory->storeWordPhys(0x00400000, 0x00004012);   // mflo $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
}

void MIPS_MOVF() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVN() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0x1);          // t1 = 1
    cpu0->setRegister(10, 0x0);         // t2 = 0
    cpu0->setRegister(11, 0x0);         // t3 = 0
    cpu0->setRegister(12, 0x0);         // t4 = 0
    memory->storeWordPhys(0x00400000, 0x0109580b);   // movn $t3, $t0, $t1   true
    memory->storeWordPhys(0x00400004, 0x010a600b);   // movn $t4, $t0, $t2   false
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(11));
    ASSERT_EQUAL(0x00000000u, cpu0->getRegister(12));
}

void MIPS_MOVT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVZ() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0x1);          // t1 = 1
    cpu0->setRegister(10, 0x0);         // t2 = 0
    cpu0->setRegister(11, 0x0);         // t3 = 0
    cpu0->setRegister(12, 0x0);         // t4 = 0
    memory->storeWordPhys(0x00400000, 0x0109580a);   // movn $t3, $t0, $t1   false
    memory->storeWordPhys(0x00400004, 0x010a600a);   // movn $t4, $t0, $t2   true
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x00000000u, cpu0->getRegister(11));
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(12));
}

void MIPS_MTHI() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setHI(0x00000000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    memory->storeWordPhys(0x00400000, 0x01000011);   // mthi $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getHI());
}

void MIPS_MTLO() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setLO(0x00000000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    memory->storeWordPhys(0x00400000, 0x01000013);   // mtlo $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getLO());
}

void MIPS_RDHWR() {
    TEST_NOT_IMPLEMENTED();
}
