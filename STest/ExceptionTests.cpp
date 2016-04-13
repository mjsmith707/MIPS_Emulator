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
    add_test("Exception Tests", "General_Exception_Processing", &General_Exception_Processing);
}

/* These tests use the testing only opcode 0x3F with the rest of the
 * instruction field used to encode the exception to use:
 * case 0x00: throw ColdResetException();
 * case 0x01: throw SoftResetException();
 * case 0x02: throw NonmaskableInterruptException();
 * case 0x03: throw MachineCheckException();
 * case 0x04: throw WatchIFException();
 * case 0x05: throw AddressErrorIFException();
 * case 0x06: throw TLBRefillIFException();
 * case 0x07: throw TLBInvalidIFException();
 * case 0x08: throw TLBExecuteInhibitException();
 * case 0x09: throw CacheErrorIFException();
 * case 0x0A: throw BusErrorIFException();
 * case 0x0B: throw CoprocessorUnusableException();
 * case 0x0C: throw ReservedInstructionException();
 * case 0x0D: throw IntegerOverflowException();
 * case 0x0E: throw TrapException();
 * case 0x0F: throw SystemCallException();
 * case 0x10: throw BreakpointException();
 * case 0x11: throw FloatingPointException();
 * case 0x12: throw Coprocessor2Exception();
 * case 0x13: throw WatchDataException();
 * case 0x14: throw AddressErrorDataException();
 * case 0x15: throw TLBRefillDataException();
 * case 0x16: throw TLBInvalidDataException();
 * case 0x17: throw TLBReadInhibitException();
 * case 0x18: throw TLBModifiedException();
 * case 0x19: throw CacheErrorDataException();
 * case 0x1A: throw BusErrorDataException();
 * case 0x1B: throw InterruptException();
*/

void Cold_Reset_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000000); // throw ColdResetException();
    cpu0->stepCPU(1);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // FIXME: test Random = TLBEntries - 1
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_PAGEMASK) & PAGEMASK_MASKX);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_PAGEGRAIN) & PAGEGRAIN_ESP);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_WIRED));
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_HWRENA));
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_ENTRYHI) & ENTRYHI_VPN2X);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_RP);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_BEV);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_TS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_SR);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_NMI);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_ERL);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_INTCTL) & INTCTL_VS);
    // FIXME: Shadowmap stuff not implemented
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_SRSCTL) & SRSCTL_HSS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_SRSCTL) & SRSCTL_ESS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_SRSCTL) & SRSCTL_PSS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_SRSCTL) & SRSCTL_CSS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_SRSMAP));
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_CAUSE) & CAUSE_DC);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_EBASE) & EBASE_EBASE);
    ASSERT_EQUAL(cop0->getRegisterReset(CO0_CONFIG0) | 0x2u, cop0->getRegister(CO0_CONFIG0));
    ASSERT_EQUAL(0x2u, cop0->getRegister(CO0_CONFIG0) & CONFIG0_K0);
    ASSERT_EQUAL(cop0->getRegisterReset(CO0_CONFIG1), cop0->getRegister(CO0_CONFIG1));
    ASSERT_EQUAL(cop0->getRegisterReset(CO0_CONFIG2), cop0->getRegister(CO0_CONFIG2));
    ASSERT_EQUAL(cop0->getRegisterReset(CO0_CONFIG3), cop0->getRegister(CO0_CONFIG3));
    // FIXME: WatchLo stuff not implemented
    // FIXME: PerfCnt stuff not implemented
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_ERROREPC));
    ASSERT_EQUAL(0xbfc00000u, cpu0->getPC());
}

void Soft_Reset_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000001); // throw SoftResetException();
    cpu0->stepCPU(1);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_PAGEMASK) & PAGEMASK_MASKX);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_PAGEGRAIN) & PAGEGRAIN_ESP);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_ENTRYHI) & ENTRYHI_VPN2X);
    ASSERT_EQUAL(0x2u, cop0->getRegister(CO0_CONFIG0) & CONFIG0_K0);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_RP);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_BEV);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_TS);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_SR);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_NMI);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_ERL);
    // FIXME: WatchLo stuff not implemented
    // FIXME: PerfCnt stuff not implemented
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_ERROREPC));
    ASSERT_EQUAL(0xbfc00000u, cpu0->getPC());
}

void Nonmaskable_Interrupt_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000002); // throw NonmaskableInterruptException();
    cpu0->stepCPU(1);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_BEV);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_TS);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_SR);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_NMI);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_ERL);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_ERROREPC));
    ASSERT_EQUAL(0xbfc00000u, cpu0->getPC());
}

void Machine_Check_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000003); // throw MachineCheckException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x18u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000003); // throw MachineCheckException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x18u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

// Watch is optional and not implemented so this test is hardly thorough
void Watch_IF_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000004); // throw WatchIFException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x17u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000004); // throw WatchIFException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x17u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Address_Error_IF_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000005); // throw AddressErrorIFException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x04u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_BADVADDR));
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000005); // throw AddressErrorIFException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x04u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_BADVADDR));
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void TLB_Refill_IF_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void TLB_Invalid_IF_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void TLB_Execute_Inhibit_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void Cache_Error_IF_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000009); // throw CacheErrorIFException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    // ASSERT_EQUAL(0x1eu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u); See pg 190 vol3, not updated during normal operation.
    ASSERT_EQUAL(0xa0000100u, cpu0->getPC());
    // NOTE: CacheErr Register and codes are optional and implementation dependent
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000009); // throw CacheErrorIFException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cpu0->stepCPU(1);
    // ASSERT_EQUAL(0x1eu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u); See pg 190 vol3, not updated during normal operation.
    ASSERT_EQUAL(0xbfc00300u, cpu0->getPC());
    // NOTE: CacheErr Register and codes are optional and implementation dependent
}

void Bus_Error_IF_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000a); // throw BusErrorIFException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x06u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000a); // throw BusErrorIFException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x06u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Coprocessor_Unusable_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000b); // throw CoprocessorUnusableException(2);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0bu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x2u, (cop0->getRegister(CO0_CAUSE) & CAUSE_CE) >> 28u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000b); // throw CoprocessorUnusableException(2);
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0bu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x2u, (cop0->getRegister(CO0_CAUSE) & CAUSE_CE) >> 28u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Reserved_Instruction_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000c); // throw ReservedInstructionException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0au, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000c); // throw ReservedInstructionException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0au, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Integer_Overflow_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000d); // throw IntegerOverflowException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0cu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000d); // throw IntegerOverflowException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0cu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Trap_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000e); // throw TrapException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void System_Call_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000f); // throw SystemCallException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x08u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000f); // throw SystemCallException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x08u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Breakpoint_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000010); // throw BreakpointException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x09u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000010); // throw BreakpointException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x09u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Floating_Point_Exception() {
    TEST_NOT_IMPLEMENTED();
}

void Coprocessor2_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000012); // throw Coprocessor2Exception();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x12u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000012); // throw Coprocessor2Exception();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x12u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

// Watch is optional and not implemented so this test is hardly thorough
void Watch_Data_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000013); // throw WatchDataException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x17u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000013); // throw WatchDataException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x17u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Address_Error_Data_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000014); // throw AddressErrorDataException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x05u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_BADVADDR));
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000014); // throw AddressErrorDataException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x05u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_BADVADDR));
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void TLB_Refill_Data_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void TLB_Invalid_Data_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void TLB_Read_Inhibit_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void TLB_Modified_Exception() {
    TEST_NOT_IMPLEMENTED(); // TODO: TLB Related stuff
}

void Cache_Error_Data_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000019); // throw CacheErrorDataException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    // ASSERT_EQUAL(0x1eu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u); See pg 190 vol3, not updated during normal operation.
    ASSERT_EQUAL(0xa0000100u, cpu0->getPC());
    // NOTE: CacheErr Register and codes are optional and implementation dependent
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc000019); // throw CacheErrorDataException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cpu0->stepCPU(1);
    // ASSERT_EQUAL(0x1eu, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u); See pg 190 vol3, not updated during normal operation.
    ASSERT_EQUAL(0xbfc00300u, cpu0->getPC());
    // NOTE: CacheErr Register and codes are optional and implementation dependent
}

void Bus_Error_Data_Exception() {
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00001a); // throw BusErrorDataException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x07u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00001a); // throw BusErrorDataException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);   // Status_bev = 1
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x07u, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}

void Interrupt_Exception() {
    // Table 6.10 pg 88 vol3
    // Status_bev, Status_exl, Cause_iv, EJTAG_probtrap
    // 0,0,0,x
    reset();
    cpu0->setPC(0x00400000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_IE);      // Status_ie = 1
    cop0->orRegisterHW(CO0_STATUS, STATUS_IM7);     // Status_im7 = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_ERL);   // Status_erl = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cop0->andRegisterHW(CO0_CAUSE, ~CAUSE_IV);      // Cause_iv = 0
    cpu0->sendInterrupt(CPU::MIPSInterrupt::HW5);
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_CAUSE) & CAUSE_IP7); // IP7 == HW5
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    // 0,0,1,x
    reset();
    cpu0->setPC(0x00400000);
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_IE);      // Status_ie = 1
    cop0->orRegisterHW(CO0_STATUS, STATUS_IM7);     // Status_im7 = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_ERL);   // Status_erl = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cop0->orRegisterHW(CO0_CAUSE, CAUSE_IV);        // Cause_iv = 1
    cpu0->sendInterrupt(CPU::MIPSInterrupt::HW5);
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_CAUSE) & CAUSE_IP7); // IP7 == HW5
    ASSERT_EQUAL(0x80000200u, cpu0->getPC());
    
    // 1,0,0,x
    reset();
    cpu0->setPC(0x00400000);
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_IE);      // Status_ie = 1
    cop0->orRegisterHW(CO0_STATUS, STATUS_IM7);     // Status_im7 = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_ERL);   // Status_erl = 0
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cop0->andRegisterHW(CO0_CAUSE, ~CAUSE_IV);      // Cause_iv = 0
    cpu0->sendInterrupt(CPU::MIPSInterrupt::HW5);
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_CAUSE) & CAUSE_IP7); // IP7 == HW5
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
    
    // 1,0,1,x
    reset();
    cpu0->setPC(0x00400000);
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_IE);      // Status_ie = 1
    cop0->orRegisterHW(CO0_STATUS, STATUS_IM7);     // Status_im7 = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_ERL);   // Status_erl = 0
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cop0->orRegisterHW(CO0_CAUSE, CAUSE_IV);        // Cause_iv = 1
    cpu0->sendInterrupt(CPU::MIPSInterrupt::HW5);
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_CAUSE) & CAUSE_IP7); // IP7 == HW5
    ASSERT_EQUAL(0xbfc00400u, cpu0->getPC());
}

// This test covers all the general exceptions, using TrapException() specifically
void General_Exception_Processing() {
    // Test for exl = 0, bev = 0, not in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000e); // throw TrapException();
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_EPC));              // EPC == address of instruction
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);        // Cause_bd == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    // Test for exl = 0, bev = 0, in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0x0eadbee0); // unconditional j 0x2ADBEE0
    memory->storeWordPhys(0x00400004, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_EPC));              // EPC == address of instruction-4
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);    // Cause_bd == 1
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    // Test for exl = 0, bev = 1, not in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_EPC));              // EPC == address of instruction-4
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);        // Cause_bd == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
    
    // Test for exl = 0, bev = 1, in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0x0eadbee0); // unconditional j 0x2ADBEE0
    memory->storeWordPhys(0x00400004, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_EXL);   // Status_exl = 0
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x00400000u, cop0->getRegister(CO0_EPC));              // EPC == address of instruction-4
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);    // Cause_bd == 1
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
    
    // Test for exl = 1, bev = 0, in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0x0eadbee0); // unconditional j 0x2ADBEE0
    memory->storeWordPhys(0x00400004, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->andRegisterHW(CO0_STATUS, ~STATUS_BEV);   // Status_bev = 0
    cop0->orRegisterHW(CO0_STATUS, STATUS_EXL);     // Status_exl = 0
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_EPC));                     // EPC == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);        // Cause_bd == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0x80000180u, cpu0->getPC());
    
    // Test for exl = 1, bev = 1, in branch delay,
    reset();
    cpu0->setPC(0x00400000);
    memory->storeWordPhys(0x00400000, 0x0eadbee0); // unconditional j 0x2ADBEE0
    memory->storeWordPhys(0x00400004, 0xfc00000e); // throw TrapException();
    cop0 = cpu0->getControlCoprocessor();
    cop0->orRegisterHW(CO0_STATUS, STATUS_BEV);     // Status_bev = 1
    cop0->orRegisterHW(CO0_STATUS, STATUS_EXL);     // Status_exl = 0
    cpu0->stepCPU(2);
    ASSERT_EQUAL(0x0du, (cop0->getRegister(CO0_CAUSE) & CAUSE_EXCCODE) >> 2u);
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_EPC));                     // EPC == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_CE);        // Cause_ce == 0
    ASSERT_EQUAL(0x0u, cop0->getRegister(CO0_CAUSE) & CAUSE_BD);        // Cause_bd == 0
    ASSERT_LESS_THAN(0x0u, cop0->getRegister(CO0_STATUS) & STATUS_EXL); // Status_exl == 1
    ASSERT_EQUAL(0xbfc00380u, cpu0->getPC());
}