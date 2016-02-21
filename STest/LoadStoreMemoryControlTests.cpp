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
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x00004021, cpu0->getControlCoprocessor());   // move $t0, $zero
    memory->storeWord(0x00400004, 0x00004821, cpu0->getControlCoprocessor());   // move $t1, $zero
    memory->storeWord(0x00400008, 0x2108ffcb, cpu0->getControlCoprocessor());   // addi $t0, -53
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp, -32
    memory->storeWord(0x00400010, 0xa3a80004, cpu0->getControlCoprocessor());   // sb $t0, 4($sp)
    memory->storeWord(0x00400014, 0x83a90004, cpu0->getControlCoprocessor());   // lb $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x1000ffe4, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LBU() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x00004021, cpu0->getControlCoprocessor());   // move $t0, $zero
    memory->storeWord(0x00400004, 0x00004821, cpu0->getControlCoprocessor());   // move $t1, $zero
    memory->storeWord(0x00400008, 0x2108ffcb, cpu0->getControlCoprocessor());   // addi $t0, -53
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp, -32
    memory->storeWord(0x00400010, 0xa3a80004, cpu0->getControlCoprocessor());   // sb $t0, 4($sp)
    memory->storeWord(0x00400014, 0x93a90004, cpu0->getControlCoprocessor());   // lbu $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0x000000cbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x1000ffe4, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LH() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c010000, cpu0->getControlCoprocessor());   // lui $at, 0x0
    memory->storeWord(0x00400004, 0x3421dead, cpu0->getControlCoprocessor());   // ori $at, $at, 0xdead
    memory->storeWord(0x00400008, 0x00014020, cpu0->getControlCoprocessor());   // add $t0, $zero, $at
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400010, 0xa7a80004, cpu0->getControlCoprocessor());   // sh $t0, 4($sp)
    memory->storeWord(0x00400014, 0x87a90004, cpu0->getControlCoprocessor());   // lh $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffdeadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalf(0x1000ffe4, &half, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LHU() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c010000, cpu0->getControlCoprocessor());   // lui $at, 0x0
    memory->storeWord(0x00400004, 0x3421dead, cpu0->getControlCoprocessor());   // ori $at, $at, 0xdead
    memory->storeWord(0x00400008, 0x00014020, cpu0->getControlCoprocessor());   // add $t0, $zero, $at
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400010, 0xa7a80004, cpu0->getControlCoprocessor());   // sh $t0, 4($sp)
    memory->storeWord(0x00400014, 0x97a90004, cpu0->getControlCoprocessor());   // lhu $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalf(0x1000ffe4, &half, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LL() {
    TEST_NOT_IMPLEMENTED();
}

// TODO: This and SW are the same test
// Would be better to test one instruction at a time
void MIPS_LW() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c01dead, cpu0->getControlCoprocessor());   // lui $at, 0xdead
    memory->storeWord(0x00400004, 0x3421beef, cpu0->getControlCoprocessor());   // ori $at, $at, 0xbeef
    memory->storeWord(0x00400008, 0x00014020, cpu0->getControlCoprocessor());   // add $t0, $zero, $at
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400010, 0xafa80004, cpu0->getControlCoprocessor());   // sw $t0, 4($sp)
    memory->storeWord(0x00400014, 0x8fa90004, cpu0->getControlCoprocessor());   // lw $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdeadbeefu, word);
    uint16_t half = 0;
    memory->readHalf(0x1000ffe4, &half, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LWL() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c08dead, cpu0->getControlCoprocessor());   // lui $t0, 0xdead
    memory->storeWord(0x00400004, 0x3508beef, cpu0->getControlCoprocessor());   // ori $t0, $t0, 0xbeef
    memory->storeWord(0x00400008, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp, $sp, -32
    memory->storeWord(0x0040000c, 0xafa80004, cpu0->getControlCoprocessor());   // sw $t0, 4($sp)
    memory->storeWord(0x00400010, 0x8ba90004, cpu0->getControlCoprocessor());   // lwl $t1, 4($sp)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    // QEMU says 0xdeadbeef (no way that's correct), Mars says 0xef000000 (LE)
    // Hummm.....
    // Manual says 0xdead
    // base = 4($sp)
    // lwl 0(base) is byte at 4(de) and 5(ad)
    // lwr 0(base) is byte at 3(00 - testing) and 4(de)
    ASSERT_EQUAL(0xdead0000u, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdeadbeefu, word);
}

void MIPS_LWR() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c08dead, cpu0->getControlCoprocessor());   // lui $t0, 0xdead
    memory->storeWord(0x00400004, 0x3508beef, cpu0->getControlCoprocessor());   // ori $t0, $t0, 0xbeef
    memory->storeWord(0x00400008, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp, $sp, -32
    memory->storeWord(0x0040000c, 0xafa80004, cpu0->getControlCoprocessor());   // sw $t0, 4($sp)
    memory->storeWord(0x00400010, 0x9ba90004, cpu0->getControlCoprocessor());   // lwr $t1, 4($sp)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    // See notes for LWL
    ASSERT_EQUAL(0x000000deu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdeadbeefu, word);
}

void MIPS_PREF() {
    TEST_NOT_IMPLEMENTED();
}

// TODO: This and LB are the same test
// Would be better to test one instruction at a time
void MIPS_SB() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x00004021, cpu0->getControlCoprocessor());   // move $t0, $zero
    memory->storeWord(0x00400004, 0x00004821, cpu0->getControlCoprocessor());   // move $t1, $zero
    memory->storeWord(0x00400008, 0x2108ffcb, cpu0->getControlCoprocessor());   // addi $t0, -53
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp, -32
    memory->storeWord(0x00400010, 0xa3a80004, cpu0->getControlCoprocessor());   // sb $t0, 4($sp)
    memory->storeWord(0x00400014, 0x83a90004, cpu0->getControlCoprocessor());   // lb $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readByte(0x1000ffe4, &byte, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_SC() {
    TEST_NOT_IMPLEMENTED();
}

// TODO: This and LH are the same test
// Would be better to test one instruction at a time
void MIPS_SH() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c010000, cpu0->getControlCoprocessor());   // lui $at, 0x0
    memory->storeWord(0x00400004, 0x3421dead, cpu0->getControlCoprocessor());   // ori $at, $at, 0xdead
    memory->storeWord(0x00400008, 0x00014020, cpu0->getControlCoprocessor());   // add $t0, $zero, $at
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400010, 0xa7a80004, cpu0->getControlCoprocessor());   // sh $t0, 4($sp)
    memory->storeWord(0x00400014, 0x87a90004, cpu0->getControlCoprocessor());   // lh $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffdeadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalf(0x1000ffe4, &half, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

// TODO: This and LW are the same test
// Would be better to test one instruction at a time
void MIPS_SW() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    memory->storeWord(0x00400000, 0x3c01dead, cpu0->getControlCoprocessor());   // lui $at, 0xdead
    memory->storeWord(0x00400004, 0x3421beef, cpu0->getControlCoprocessor());   // ori $at, $at, 0xbeef
    memory->storeWord(0x00400008, 0x00014020, cpu0->getControlCoprocessor());   // add $t0, $zero, $at
    memory->storeWord(0x0040000c, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400010, 0xafa80004, cpu0->getControlCoprocessor());   // sw $t0, 4($sp)
    memory->storeWord(0x00400014, 0x8fa90004, cpu0->getControlCoprocessor());   // lw $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWord(0x1000ffe4, &word, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0xdeadbeefu, word);
    uint16_t half = 0;
    memory->readHalf(0x1000ffe4, &half, cpu0->getControlCoprocessor());
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_SWL() {
    /*
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(29, 0x10010000);  // Init Stack pointer
    cpu0->setRegister(8, 0xdeadbeef);   // $t0 = 0xdeadbeef
    memory->storeWord(0x00400000, 0x23bdffe0, cpu0->getControlCoprocessor());   // addi $sp $sp, -32
    memory->storeWord(0x00400004, 0x23bdffe0, cpu0->getControlCoprocessor());   // swl $t0, 2($sp)
    cpu0->stepCPU(1);
    */
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