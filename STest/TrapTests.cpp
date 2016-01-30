//
//  TrapTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "TrapTests.h"
using namespace STest;

void add_trap_tests() {
    add_test("CPU Trap Tests", "MIPS_BREAK", &MIPS_BREAK);
    add_test("CPU Trap Tests", "MIPS_SYSCALL", &MIPS_SYSCALL);
    add_test("CPU Trap Tests", "MIPS_TEQ", &MIPS_TEQ);
    add_test("CPU Trap Tests", "MIPS_TEQI", &MIPS_TEQI);
    add_test("CPU Trap Tests", "MIPS_TGE", &MIPS_TGE);
    add_test("CPU Trap Tests", "MIPS_TGEI", &MIPS_TGEI);
    add_test("CPU Trap Tests", "MIPS_TGEIU", &MIPS_TGEIU);
    add_test("CPU Trap Tests", "MIPS_TGEU", &MIPS_TGEU);
    add_test("CPU Trap Tests", "MIPS_TLT", &MIPS_TLT);
    add_test("CPU Trap Tests", "MIPS_TLTI", &MIPS_TLTI);
    add_test("CPU Trap Tests", "MIPS_TLTIU", &MIPS_TLTIU);
    add_test("CPU Trap Tests", "MIPS_TLTU", &MIPS_TLTU);
    add_test("CPU Trap Tests", "MIPS_TNE", &MIPS_TNE);
    add_test("CPU Trap Tests", "MIPS_TNEI", &MIPS_TNEI);
}

void MIPS_BREAK() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SYSCALL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TEQ() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TEQI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGE() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEIU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TGEU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTIU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLTU() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TNE() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TNEI() {
    TEST_NOT_IMPLEMENTED();
}