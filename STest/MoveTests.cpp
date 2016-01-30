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

void MIPS_MFHI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MFLO() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVF() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVN() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MOVZ() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MTHI() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_MTLO() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_RDHWR() {
    TEST_NOT_IMPLEMENTED();
}
