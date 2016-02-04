//
//  LoadStoreMemoryControlTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "LoadStoreMemoryControlTests.h"
using namespace STest;

void add_load_store_memory_control_tests() {
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LB", &MIPS_LB);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LBU", &MIPS_LBU);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LH", &MIPS_LH);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LHU", &MIPS_LHU);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LL", &MIPS_LL);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LW", &MIPS_LW);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LWL", &MIPS_LWL);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_LWR", &MIPS_LWR);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_PREF", &MIPS_PREF);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SB", &MIPS_SB);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SC", &MIPS_SC);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SH", &MIPS_SH);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SW", &MIPS_SW);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SWL", &MIPS_SWL);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SWR", &MIPS_SWR);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SYNC", &MIPS_SYNC);
    add_test("CPU Load, Store, and Memory Control Tests", "MIPS_SYNCI", &MIPS_SYNCI);
}

void MIPS_LB() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ffcb, cpu0->getControlCoprocessor());   // addi	v0,zero,-53
    memory->storeWord(0x00400004, 0x3c011001, cpu0->getControlCoprocessor());   // lui	at,0x1001
    memory->storeWord(0x00400008, 0x34240020, cpu0->getControlCoprocessor());   // ori	a0,at,0x20
    memory->storeWord(0x0040000c, 0xa082ffe0, cpu0->getControlCoprocessor());   // sb	v0,-32(a0)
    memory->storeWord(0x00400010, 0x8083ffe0, cpu0->getControlCoprocessor());   // lb	v1,-32(a0)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(2));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(3));
    uint32_t word = 0;
    memory->readWord(0x10010000, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x10010000, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LBU() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ffcb, cpu0->getControlCoprocessor());   // addi	v0,zero,-53
    memory->storeWord(0x00400004, 0x3c011001, cpu0->getControlCoprocessor());   // lui	at,0x1001
    memory->storeWord(0x00400008, 0x34240020, cpu0->getControlCoprocessor());   // ori	a0,at,0x20
    memory->storeWord(0x0040000c, 0xa082ffe0, cpu0->getControlCoprocessor());   // sb	v0,-32(a0)
    memory->storeWord(0x00400010, 0x9083ffe0, cpu0->getControlCoprocessor());   // lbu	v1,-32(a0)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(2));
    ASSERT_EQUAL(0x000000cbu, cpu0->getRegister(3));
    uint32_t word = 0;
    memory->readWord(0x10010000, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x10010000, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LH() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_LHU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_LL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_LW() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_LWL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_LWR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_PREF() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SB() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ffcb, cpu0->getControlCoprocessor());   // addi	v0,zero,-53
    memory->storeWord(0x00400004, 0x3c011001, cpu0->getControlCoprocessor());   // lui	at,0x1001
    memory->storeWord(0x00400008, 0x34240020, cpu0->getControlCoprocessor());   // ori	a0,at,0x20
    memory->storeWord(0x0040000c, 0xa082ffe0, cpu0->getControlCoprocessor());   // sb	v0,-32(a0)
    memory->storeWord(0x00400010, 0x8083ffe0, cpu0->getControlCoprocessor());   // lb	v1,-32(a0)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(2));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(3));
    uint32_t word = 0;
    memory->readWord(0x10010000, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x10010000, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_SC() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SH() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SW() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SWL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SWR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SYNC() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SYNCI() {
    TEST_NOT_IMPLEMENTED();
}