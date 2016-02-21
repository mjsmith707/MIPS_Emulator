//
//  InsertExtractTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/29/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "InsertExtractTests.h"
using namespace STest;

void add_insert_extract_tests() {
    add_test("CPU Insert/Extract Tests", "MIPS_EXT", &MIPS_EXT);
    add_test("CPU Insert/Extract Tests", "MIPS_INS", &MIPS_INS);
    add_test("CPU Insert/Extract Tests", "MIPS_WSBH", &MIPS_WSBH);
}

void MIPS_EXT() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0x0);          // t1 = 0x0
    memory->storeWord(0x00400000, 0x7d097c00, cpu0->getControlCoprocessor());   // ext $t1, $t0, 16, 16
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0000deadu, cpu0->getRegister(9));
}

void MIPS_INS() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0xf0f0ffff);   // t1 = 0xf0f0ffff
    memory->storeWord(0x00400000, 0x7d09fc04, cpu0->getControlCoprocessor());   // ins $t1, $t0, 16, 16
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xbeefffffu, cpu0->getRegister(9));
}

void MIPS_WSBH() {
    reset();
    cpu0->setPC(0x00400000);
    cpu0->setRegister(8, 0xdeadbeef);   // t0 = 0xdeadbeef
    cpu0->setRegister(9, 0x0);          // t1 = 0x0
    memory->storeWord(0x00400000, 0x7c0848a0, cpu0->getControlCoprocessor());   // wsbh $t1, $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xaddeefbeu, cpu0->getRegister(9));
}