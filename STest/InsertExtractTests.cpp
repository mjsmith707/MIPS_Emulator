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
    TEST_NOT_IMPLEMENTED();
}

void MIPS_INS() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_WSBH() {
    TEST_NOT_IMPLEMENTED();
}