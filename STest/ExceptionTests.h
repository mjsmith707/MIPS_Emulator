//
//  ExceptionTests.h
//  MIPS_Emulator
//
//  Created by Matt on 3/11/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef ExceptionTests_h
#define ExceptionTests_h

#include "MIPS_Tests_Common.h"

void add_exception_tests();

void Cold_Reset_Exception();
void Soft_Reset_Exception();
void Nonmaskable_Interrupt_Exception();
void Machine_Check_Exception();
void Watch_IF_Exception();
void Address_Error_IF_Exception();
void TLB_Refill_IF_Exception();
void TLB_Invalid_IF_Exception();
void TLB_Execute_Inhibit_Exception();
void Cache_Error_IF_Exception();
void Bus_Error_IF_Exception();
void Coprocessor_Unusable_Exception();
void Reserved_Instruction_Exception();
void Integer_Overflow_Exception();
void Trap_Exception();
void System_Call_Exception();
void Breakpoint_Exception();
void Floating_Point_Exception();
void Coprocessor2_Exception();
void Watch_Data_Exception();
void Address_Error_Data_Exception();
void TLB_Refill_Data_Exception();
void TLB_Invalid_Data_Exception();
void TLB_Read_Inhibit_Exception();
void TLB_Modified_Exception();
void Cache_Error_Data_Exception();
void Bus_Error_Data_Exception();
void Interrupt_Exception();
void General_Exception_Processing();

#endif /* ExceptionTests_h */
