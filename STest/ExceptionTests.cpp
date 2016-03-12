//
//  ExceptionTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 3/11/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ExceptionTests.h"
using namespace STest;

void add_exception_tests() {
    add_test("Exception Tests", "Cold_Reset_Exception", &Cold_Reset_Exception);
    add_test("Exception Tests", "Soft_Reset_Exception", &Soft_Reset_Exception);
    add_test("Exception Tests", "Nonmaskable_Interrupt_Exception", &Nonmaskable_Interrupt_Exception);
    add_test("Exception Tests", "Machine_Check_Exception", &Machine_Check_Exception);
    add_test("Exception Tests", "Watch_IF_Exception", &Watch_IF_Exception);
    add_test("Exception Tests", "Address_Error_IF_Exception", &Address_Error_IF_Exception);
    add_test("Exception Tests", "TLB_Refill_IF_Exception", &TLB_Refill_IF_Exception);
    add_test("Exception Tests", "TLB_Invalid_IF_Exception", &TLB_Invalid_IF_Exception);
    add_test("Exception Tests", "TLB_Execute_Inhibit_Exception", &TLB_Execute_Inhibit_Exception);
    add_test("Exception Tests", "Cache_Error_IF_Exception", &Cache_Error_IF_Exception);
    add_test("Exception Tests", "Bus_Error_IF_Exception", &Bus_Error_IF_Exception);
    add_test("Exception Tests", "Coprocessor_Unusable_Exception", &Coprocessor_Unusable_Exception);
    add_test("Exception Tests", "Reserved_Instruction_Exception", &Reserved_Instruction_Exception);
    add_test("Exception Tests", "Integer_Overflow_Exception", &Integer_Overflow_Exception);
    add_test("Exception Tests", "Trap_Exception", &Trap_Exception);
    add_test("Exception Tests", "System_Call_Exception", &System_Call_Exception);
    add_test("Exception Tests", "Breakpoint_Exception", &Breakpoint_Exception);
    add_test("Exception Tests", "Floating_Point_Exception", &Floating_Point_Exception);
    add_test("Exception Tests", "Coprocessor2_Exception", &Coprocessor2_Exception);
    add_test("Exception Tests", "Watch_Data_Exception", &Watch_Data_Exception);
    add_test("Exception Tests", "Address_Error_Data_Exception", &Address_Error_Data_Exception);
    add_test("Exception Tests", "TLB_Refill_Data_Exception", &TLB_Refill_Data_Exception);
    add_test("Exception Tests", "TLB_Invalid_Data_Exception", &TLB_Invalid_Data_Exception);
    add_test("Exception Tests", "TLB_Read_Inhibit_Exception", &TLB_Read_Inhibit_Exception);
    add_test("Exception Tests", "TLB_Modified_Exception", &TLB_Modified_Exception);
    add_test("Exception Tests", "Cache_Error_Data_Exception", &Cache_Error_Data_Exception);
    add_test("Exception Tests", "Bus_Error_Data_Exception", &Bus_Error_Data_Exception);
    add_test("Exception Tests", "Interrupt_Exception", &Interrupt_Exception);
}

void Cold_Reset_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Soft_Reset_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Nonmaskable_Interrupt_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Machine_Check_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Watch_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Address_Error_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Refill_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Invalid_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Execute_Inhibit_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Cache_Error_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Bus_Error_IF_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Coprocessor_Unusable_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Reserved_Instruction_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Integer_Overflow_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Trap_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void System_Call_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Breakpoint_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Floating_Point_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Coprocessor2_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Watch_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Address_Error_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Refill_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Invalid_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Read_Inhibit_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void TLB_Modified_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Cache_Error_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Bus_Error_Data_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Interrupt_Exception() {
    TEST_NOT_IMPLEMENTED();
}