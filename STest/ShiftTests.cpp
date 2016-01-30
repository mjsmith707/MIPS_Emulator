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
    TEST_NOT_IMPLEMENTED();
}

void MIPS_ROTRV() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SLLV() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SRA() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SRAV() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SRL() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_SRLV() {
    TEST_NOT_IMPLEMENTED();
}