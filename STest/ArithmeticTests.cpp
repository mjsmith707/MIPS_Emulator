//
//  ArithmeticTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/28/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ArithmeticTests.h"
using namespace STest;

// Not ideal but the macro way is horrific to write
void add_arithmetic_tests() {
    add_test("CPU Arithmetic Tests", "MIPS_ADD", &MIPS_ADD);
    add_test("CPU Arithmetic Tests", "MIPS_ADDI", &MIPS_ADDI);
    add_test("CPU Arithmetic Tests", "MIPS_ADDIU", &MIPS_ADDIU);
    add_test("CPU Arithmetic Tests", "MIPS_ADDU", &MIPS_ADDU);
    add_test("CPU Arithmetic Tests", "MIPS_CLO", &MIPS_CLO);
    add_test("CPU Arithmetic Tests", "MIPS_CLZ", &MIPS_CLZ);
    add_test("CPU Arithmetic Tests", "MIPS_DIV", &MIPS_DIV);
    add_test("CPU Arithmetic Tests", "MIPS_DIVU", &MIPS_DIVU);
    add_test("CPU Arithmetic Tests", "MIPS_MADD", &MIPS_MADD);
    add_test("CPU Arithmetic Tests", "MIPS_MADDU", &MIPS_MADDU);
    add_test("CPU Arithmetic Tests", "MIPS_MSUB", &MIPS_MSUB);
    add_test("CPU Arithmetic Tests", "MIPS_MSUBU", &MIPS_MSUBU);
    add_test("CPU Arithmetic Tests", "MIPS_MUL", &MIPS_MUL);
    add_test("CPU Arithmetic Tests", "MIPS_MULT", &MIPS_MULT);
    add_test("CPU Arithmetic Tests", "MIPS_MULTU", &MIPS_MULTU);
    add_test("CPU Arithmetic Tests", "MIPS_SEB", &MIPS_SEB);
    add_test("CPU Arithmetic Tests", "MIPS_SEH", &MIPS_SEH);
    add_test("CPU Arithmetic Tests", "MIPS_SLT", &MIPS_SLT);
    add_test("CPU Arithmetic Tests", "MIPS_SLTI", &MIPS_SLTI);
    add_test("CPU Arithmetic Tests", "MIPS_SLTIU", &MIPS_SLTIU);
    add_test("CPU Arithmetic Tests", "MIPS_SLTU", &MIPS_SLTU);
    add_test("CPU Arithmetic Tests", "MIPS_SUB", &MIPS_SUB);
    add_test("CPU Arithmetic Tests", "MIPS_SUBU", &MIPS_SUBU);
}

void MIPS_ADD() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000010, 0x00436020, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000014, 0x00456820, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000018, 0x00837020, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000001C, 0x00857820, cpu0->getControlCoprocessor());
    cpu0->stepCPU(8);
    ASSERT_EQUAL(700u, cpu0->getRegister(12));
    ASSERT_EQUAL(-300, (int32_t)cpu0->getRegister(13));
    ASSERT_EQUAL(300u, cpu0->getRegister(14));
    ASSERT_EQUAL(-700, (int32_t)cpu0->getRegister(15));
}

void MIPS_ADDI() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000010, 0x204801f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000014, 0x2049fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000018, 0x208a01f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000001C, 0x208bfe0c, cpu0->getControlCoprocessor());
    cpu0->stepCPU(8);
    ASSERT_EQUAL(200u, cpu0->getRegister(2));
    ASSERT_EQUAL(500u, cpu0->getRegister(3));
    ASSERT_EQUAL(-200, (int32_t)cpu0->getRegister(4));
    ASSERT_EQUAL(-500, (int32_t)cpu0->getRegister(5));
    ASSERT_EQUAL(700u, cpu0->getRegister(8));
    ASSERT_EQUAL(-300, (int32_t)cpu0->getRegister(9));
    ASSERT_EQUAL(300u, cpu0->getRegister(10));
    ASSERT_EQUAL(-700, (int32_t)cpu0->getRegister(11));
}

void MIPS_ADDIU() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000010, 0x244801f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000014, 0x2449fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000018, 0x248a01f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000001C, 0x248bfe0c, cpu0->getControlCoprocessor());
    cpu0->stepCPU(8);
    ASSERT_EQUAL(700u, cpu0->getRegister(8));
    ASSERT_EQUAL(-300, (int32_t)cpu0->getRegister(9));
    ASSERT_EQUAL(300u, cpu0->getRegister(10));
    ASSERT_EQUAL(-700, (int32_t)cpu0->getRegister(11));
}

void MIPS_ADDU() {
    reset();
    cpu0->setPC(0x00000000);
    memory->storeWord(0x00000000, 0x200200c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000004, 0x200301f4, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000008, 0x2004ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000000C, 0x2005fe0c, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000010, 0x00434021, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000014, 0x00454821, cpu0->getControlCoprocessor());
    memory->storeWord(0x00000018, 0x00835021, cpu0->getControlCoprocessor());
    memory->storeWord(0x0000001C, 0x00855821, cpu0->getControlCoprocessor());
    cpu0->stepCPU(8);
    ASSERT_EQUAL(700u, cpu0->getRegister(8));
    ASSERT_EQUAL(-300, (int32_t)cpu0->getRegister(9));
    ASSERT_EQUAL(300u, cpu0->getRegister(10));
    ASSERT_EQUAL(-700, (int32_t)cpu0->getRegister(11));
}

void MIPS_CLO() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400008, 0x70402021, cpu0->getControlCoprocessor());
    memory->storeWord(0x0040000C, 0x70602821, cpu0->getControlCoprocessor());
    cpu0->stepCPU(4);
    ASSERT_EQUAL(0x00000018u, cpu0->getRegister(4));
    ASSERT_EQUAL(0x00000000u, cpu0->getRegister(5));
}

void MIPS_CLZ() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ff38, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400008, 0x70402020, cpu0->getControlCoprocessor());
    memory->storeWord(0x0040000C, 0x70602820, cpu0->getControlCoprocessor());
    cpu0->stepCPU(4);
    ASSERT_EQUAL(0x00000000u, cpu0->getRegister(4));
    ASSERT_EQUAL(0x00000018u, cpu0->getRegister(5));
}

void MIPS_DIV() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ffcb, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400008, 0x0043001a, cpu0->getControlCoprocessor());
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0xffffffcbu, cpu0->getHI());
    ASSERT_EQUAL(0x00000000u, cpu0->getLO());
    memory->storeWord(0x0040000C, 0x0062001a, cpu0->getControlCoprocessor());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x00000029u, cpu0->getHI());
    ASSERT_EQUAL(0xfffffffdu, cpu0->getLO());
}

void MIPS_DIVU() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWord(0x00400000, 0x2002ffcb, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400004, 0x200300c8, cpu0->getControlCoprocessor());
    memory->storeWord(0x00400008, 0x0043001b, cpu0->getControlCoprocessor());
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0x0000002bu, cpu0->getHI());
    ASSERT_EQUAL(0x0147ae14u, cpu0->getLO());
    memory->storeWord(0x0040000C, 0x0062001b, cpu0->getControlCoprocessor());
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x000000c8u, cpu0->getHI());
    ASSERT_EQUAL(0x00000000u, cpu0->getLO());
}

void MIPS_MADD() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MADDU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MSUB() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MSUBU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MUL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MULT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MULTU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SEB() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SEH() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLTI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLTIU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLTU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SUB() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SUBU() {
    TEST_NOT_IMPLEMENTED();
}