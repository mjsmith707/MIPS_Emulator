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
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x00004021);   // move $t0, $zero
    memory->storeWordPhys(0xA0000004, 0x00004821);   // move $t1, $zero
    memory->storeWordPhys(0xA0000008, 0x2108ffcb);   // addi $t0, -53
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa3a80004);   // sb $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x83a90004);   // lb $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readBytePhys(0xA000ffe4, &byte);
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LBU() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x00004021);   // move $t0, $zero
    memory->storeWordPhys(0xA0000004, 0x00004821);   // move $t1, $zero
    memory->storeWordPhys(0xA0000008, 0x2108ffcb);   // addi $t0, -53
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa3a80004);   // sb $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x93a90004);   // lbu $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0x000000cbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readBytePhys(0xA000ffe4, &byte);
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_LH() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c010000);   // lui $at, 0x0
    memory->storeWordPhys(0xA0000004, 0x3421dead);   // ori $at, $at, 0xdead
    memory->storeWordPhys(0xA0000008, 0x00014020);   // add $t0, $zero, $at
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa7a80004);   // sh $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x87a90004);   // lh $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffdeadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalfPhys(0xA000ffe4, &half);
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LHU() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c010000);   // lui $at, 0x0
    memory->storeWordPhys(0xA0000004, 0x3421dead);   // ori $at, $at, 0xdead
    memory->storeWordPhys(0xA0000008, 0x00014020);   // add $t0, $zero, $at
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa7a80004);   // sh $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x97a90004);   // lhu $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalfPhys(0xA000ffe4, &half);
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LL() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    ASSERT_EQUAL(false, memory->getFrameBool(0xA0010000));  // reset() should be clearing this bit every time.
    memory->storeWordPhys(0xA0010000, 0xDEADBEEF);          // Store value, frame's LLbit is now dirty
    ASSERT_EQUAL(true, memory->getFrameBool(0xA0010000));
    memory->storeWordPhys(0xA0000000, 0xc3a80000);          // ll $t0, 0($sp)
    cpu0->stepCPU(1);                                       // Frame's LLbit is now cleared
    ASSERT_EQUAL(true, cpu0->getLLBit());                   // RWM sequence started
    ASSERT_EQUAL(0xA0010u, cpu0->getLLPFN());
    ASSERT_EQUAL(0xDEADBEEFu, cpu0->getRegister(8));
    ASSERT_EQUAL(false, memory->getFrameBool(0xA0010000));
}

// TODO: This and SW are the same test
// Would be better to test one instruction at a time
void MIPS_LW() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c01dead);   // lui $at, 0xdead
    memory->storeWordPhys(0xA0000004, 0x3421beef);   // ori $at, $at, 0xbeef
    memory->storeWordPhys(0xA0000008, 0x00014020);   // add $t0, $zero, $at
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000010, 0xafa80004);   // sw $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x8fa90004);   // lw $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdeadbeefu, word);
    uint16_t half = 0;
    memory->readHalfPhys(0xA000ffe4, &half);
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_LWL() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c08dead);   // lui $t0, 0xdead
    memory->storeWordPhys(0xA0000004, 0x3508beef);   // ori $t0, $t0, 0xbeef
    memory->storeWordPhys(0xA0000008, 0x23bdffe0);   // addi $sp, $sp, -32
    memory->storeWordPhys(0xA000000c, 0xafa80004);   // sw $t0, 4($sp)
    memory->storeWordPhys(0xA0000010, 0x8ba90004);   // lwl $t1, 4($sp)
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
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdeadbeefu, word);
}

void MIPS_LWR() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c08dead);   // lui $t0, 0xdead
    memory->storeWordPhys(0xA0000004, 0x3508beef);   // ori $t0, $t0, 0xbeef
    memory->storeWordPhys(0xA0000008, 0x23bdffe0);   // addi $sp, $sp, -32
    memory->storeWordPhys(0xA000000c, 0xafa80004);   // sw $t0, 4($sp)
    memory->storeWordPhys(0xA0000010, 0x9ba90004);   // lwr $t1, 4($sp)
    cpu0->stepCPU(5);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    // See notes for LWL
    ASSERT_EQUAL(0x000000deu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdeadbeefu, word);
}

void MIPS_PREF() {
    TEST_NOT_IMPLEMENTED();
}

// TODO: This and LB are the same test
// Would be better to test one instruction at a time
void MIPS_SB() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x00004021);   // move $t0, $zero
    memory->storeWordPhys(0xA0000004, 0x00004821);   // move $t1, $zero
    memory->storeWordPhys(0xA0000008, 0x2108ffcb);   // addi $t0, -53
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa3a80004);   // sb $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x83a90004);   // lb $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffffcbu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xcb000000u, word);
    uint8_t byte = 0;
    memory->readBytePhys(0xA000ffe4, &byte);
    ASSERT_EQUAL(0x000000cbu, (uint32_t)byte);
}

void MIPS_SC() {
    // Successful SC
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    cpu0->setRegister(8, 0xDEADBEEF);   // $t0 = 0xDEADBEEF
    memory->storeWordPhys(0xA0000000, 0xe3a80000);          // sc $t0, 0($sp)
    ASSERT_EQUAL(false, memory->getFrameBool(0xA0010000));  // reset() should be clearing this bit every time.
    cpu0->setLLBit(true);                                   // Begin RWM sequence (normally set by LL instruction)
    cpu0->setLLPFN(0xA0010);
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x1u, cpu0->getRegister(8));   // Successful
    ASSERT_EQUAL(false, cpu0->getLLBit());      // RWM sequence ended
    ASSERT_EQUAL(true, memory->getFrameBool(0xA0010000));   // Frame is now dirty
    uint32_t word = 0;
    memory->readWordPhys(0xA0010000, &word);
    ASSERT_EQUAL(0xDEADBEEFu, word);
    
    // Failed SC
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    cpu0->setRegister(8, 0xDEADBEEF);   // $t0 = 0xDEADBEEF
    memory->storeWordPhys(0xA0000000, 0xe3a80000);          // sc $t0, 0($sp)
    ASSERT_EQUAL(false, memory->getFrameBool(0xA0010000));  // reset() should be clearing this bit every time.
    cpu0->setLLBit(true);                                   // Begin RWM sequence (normally set by LL instruction)
    cpu0->setLLPFN(0xA0010);
    memory->storeWordPhys(0xA0010000, 0xBEEFDEAD);          // Frame is now dirty
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0u, cpu0->getRegister(8));   // Failure
    ASSERT_EQUAL(true, cpu0->getLLBit());       // RWM sequence not ended
    word = 0;
    memory->readWordPhys(0xA0010000, &word);
    ASSERT_EQUAL(0xBEEFDEADu, word);    // Should remain unmodified
}

// TODO: This and LH are the same test
// Would be better to test one instruction at a time
void MIPS_SH() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c010000);   // lui $at, 0x0
    memory->storeWordPhys(0xA0000004, 0x3421dead);   // ori $at, $at, 0xdead
    memory->storeWordPhys(0xA0000008, 0x00014020);   // add $t0, $zero, $at
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000010, 0xa7a80004);   // sh $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x87a90004);   // lh $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xffffdeadu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdead0000u, word);
    uint16_t half = 0;
    memory->readHalfPhys(0xA000ffe4, &half);
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

// TODO: This and LW are the same test
// Would be better to test one instruction at a time
void MIPS_SW() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    memory->storeWordPhys(0xA0000000, 0x3c01dead);   // lui $at, 0xdead
    memory->storeWordPhys(0xA0000004, 0x3421beef);   // ori $at, $at, 0xbeef
    memory->storeWordPhys(0xA0000008, 0x00014020);   // add $t0, $zero, $at
    memory->storeWordPhys(0xA000000c, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000010, 0xafa80004);   // sw $t0, 4($sp)
    memory->storeWordPhys(0xA0000014, 0x8fa90004);   // lw $t1, 4($sp)
    cpu0->stepCPU(6);
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(8));
    ASSERT_EQUAL(0xdeadbeefu, cpu0->getRegister(9));
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xdeadbeefu, word);
    uint16_t half = 0;
    memory->readHalfPhys(0xA000ffe4, &half);
    ASSERT_EQUAL(0x0000deadu, (uint32_t)half);
}

void MIPS_SWL() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    cpu0->setRegister(8, 0xdeadbeef);   // $t0 = 0xdeadbeef
    memory->storeWordPhys(0xA0000000, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000004, 0xaba80002);   // swl $t0, 2($sp)
    cpu0->stepCPU(2);
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe0, &word);
    ASSERT_EQUAL(0x0000deadu, word);
}

void MIPS_SWR() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(29, 0xA0010000);  // Init Stack pointer
    cpu0->setRegister(8, 0xdeadbeef);   // $t0 = 0xdeadbeef
    memory->storeWordPhys(0xA0000000, 0x23bdffe0);   // addi $sp $sp, -32
    memory->storeWordPhys(0xA0000004, 0xbba80005);   // swr $t0, 5($sp)
    cpu0->stepCPU(2);
    uint32_t word = 0;
    memory->readWordPhys(0xA000ffe4, &word);
    ASSERT_EQUAL(0xbeef0000u, word);
}

void MIPS_SYNC() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SYNCI() {
    TEST_NOT_IMPLEMENTED();
}