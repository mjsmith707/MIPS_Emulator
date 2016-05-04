//
//  PrivilegedInstructionTests.cpp
//  MIPS_Emulator
//
//  Created by Matt on 3/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "PrivilegedInstructionTests.h"
using namespace STest;

void add_privileged_instruction_tests() {
    add_test("CPU Privileged Instruction Tests", "MIPS_DI", &MIPS_DI);
    add_test("CPU Privileged Instruction Tests", "MIPS_EI", &MIPS_EI);
    add_test("CPU Privileged Instruction Tests", "MIPS_ERET", &MIPS_ERET);
    add_test("CPU Privileged Instruction Tests", "MIPS_MFC0", &MIPS_MFC0);
    add_test("CPU Privileged Instruction Tests", "MIPS_MTC0", &MIPS_MTC0);
    add_test("CPU Privileged Instruction Tests", "MIPS_RDPGPR", &MIPS_RDPGPR);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBP", &MIPS_TLBP);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBR", &MIPS_TLBR);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBWI", &MIPS_TLBWI);
    add_test("CPU Privileged Instruction Tests", "MIPS_TLBWR", &MIPS_TLBWR);
    add_test("CPU Privileged Instruction Tests", "MIPS_WAIT", &MIPS_WAIT);
    add_test("CPU Privileged Instruction Tests", "MIPS_WRPGPR", &MIPS_WRPGPR);
    add_test("CPU Privileged Instruction Tests", "TEST_VADDR_TRANSLATION", &TEST_VADDR_TRANSLATION);
}

void MIPS_DI() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(8, 0); // $t0 = 0
    // Enable Interrupts
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) | STATUS_IE);
    uint32_t status = cop0->getRegister(CO0_STATUS);
    memory->storeWordPhys(0xA0000000, 0x41686000); // di $t0
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0u, cop0->getRegister(CO0_STATUS) & STATUS_IE);
    ASSERT_EQUAL(status, cpu0->getRegister(8));
}

void MIPS_EI() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(8, 0); // $t0 = 0
    // Disable Interrupts
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_IE));
    uint32_t status = cop0->getRegister(CO0_STATUS);
    memory->storeWordPhys(0xA0000000, 0x41686020); // ei $t0
    cpu0->stepCPU(1);
    ASSERT_LESS_THAN(0u, cop0->getRegister(CO0_STATUS) & STATUS_IE);
    ASSERT_EQUAL(status, cpu0->getRegister(8));
}

void MIPS_ERET() {
    // Status_erl = 1
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set Status_erl = 1
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) | STATUS_ERL);
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0xDEADBEE0);
    memory->storeWordPhys(0xA0000000, 0x42000018); // eret
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEE0u, cpu0->getPC());
    
    // Status_erl = 0
    reset();
    cpu0->setPC(0xA0000000);
    cop0 = cpu0->getControlCoprocessor();
    // Set Status_erl = 0
    cop0->setRegisterHW(CO0_STATUS, cop0->getRegister(CO0_STATUS) & ~(STATUS_ERL));
    // Set EPC
    cop0->setRegisterHW(CO0_EPC, 0xDEADBEE0);
    memory->storeWordPhys(0xA0000000, 0x42000018); // eret
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEE0u, cpu0->getPC());
}

// More thorough coprocessor 0 register testing will be done elsewhere
void MIPS_MFC0() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(8, 0); // $t0 = 0
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0xDEADBEEF);
    memory->storeWordPhys(0xA0000000, 0x4008f000); // mfc0 $t0, c0_errorepc
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEEFu, cpu0->getRegister(8));
}

// More thorough coprocessor 0 register testing will be done elsewhere
void MIPS_MTC0() {
    reset();
    cpu0->setPC(0xA0000000);
    cpu0->setRegister(8, 0xDEADBEEF); // $t0 = 0xDEADBEEF
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    // Set ErrorEPC
    cop0->setRegisterHW(CO0_ERROREPC, 0);
    memory->storeWordPhys(0xA0000000, 0x4088f000); // mtc0 $t0, c0_errorepc
    cpu0->stepCPU(1);
    ASSERT_EQUAL(0xDEADBEEFu, cop0->getRegister(CO0_ERROREPC));
}

void MIPS_RDPGPR() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_TLBP() {
    // Insert an entry into the TLB
    // This is basically repeating TLBWI, so if it fails,
    // the result of this is meaningless.
    
    /* Begin TLBWI */
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    
    // Set up registers for TLB write
    // Index = 5
    cop0->setRegisterHW(CO0_INDEX, 5);
    // Page Size = 4KB
    // This is pulled directly from Linux:
    //  arch/mips/include/asm/mipsregs.h PM_4K
    // although Pagemask is locked to 0.
    // This is for bits 12..11 and only has an
    // affect if Config3_sp = 1 & PageGrain_esp = 1,
    // aka if support for 1KB pages exists.
    cop0->setRegisterHW(CO0_PAGEMASK, 0x1800);
    // EntryHI_vpn2 = 0x00400000
    cop0->setRegisterHW(CO0_ENTRYHI, 0x00400000);
    // EntryHi_asid = 0xA
    cop0->orRegisterHW(CO0_ENTRYHI, 0x0A);
    // EntryLo0_PFN = 0x0
    // EntryLo0_C = 3 (Cacheable)
    // EntryLo0_D = 1 (Writable)
    // EntryLo0_V = 1 (Valid)
    // EntryLo0_G = 0 (Not global)
    // Remap to pfn 0 for testing
    cop0->setRegisterHW(CO0_ENTRYLO0, 0x1E);
    // EntryLo1_PFN = 0x1
    // EntryLo1_C = 3 (Cacheable)
    // EntryLo1_D = 1 (Writable)
    // EntryLo1_V = 1 (Valid)
    // EntryLo1_G = 0 (Not global)
    // Remap to pfn 1 for testing
    cop0->setRegisterHW(CO0_ENTRYLO1, 0x5E);
    
    memory->storeWordPhys(0xA0000000, 0x42000002);  // tlbwi
    cpu0->stepCPU(1);
    /* End TLBWI */
    
    
    // Run TLBP
    memory->storeWordPhys(0xA0000004, 0x42000008);  // tlbp
    cpu0->stepCPU(1);
    
    ASSERT_EQUAL(0x5u, cop0->getRegister(CO0_INDEX));
}

void MIPS_TLBR() {
    /* Begin TLBWI */
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    
    // Set up registers for TLB write
    // Index = 0
    cop0->setRegisterHW(CO0_INDEX, 0);
    // Page Size = 4KB
    // This is pulled directly from Linux:
    //  arch/mips/include/asm/mipsregs.h PM_4K
    // although Pagemask is locked to 0.
    // This is for bits 12..11 and only has an
    // affect if Config3_sp = 1 & PageGrain_esp = 1,
    // aka if support for 1KB pages exists.
    cop0->setRegisterHW(CO0_PAGEMASK, 0x1800);
    // EntryHI_vpn2 = 0x00400000
    cop0->setRegisterHW(CO0_ENTRYHI, 0x00400000);
    // EntryHi_asid = 0xA
    cop0->orRegisterHW(CO0_ENTRYHI, 0x0A);
    // EntryLo0_PFN = 0x0
    // EntryLo0_C = 3 (Cacheable)
    // EntryLo0_D = 1 (Writable)
    // EntryLo0_V = 1 (Valid)
    // EntryLo0_G = 0 (Not global)
    // Remap to pfn 0 for testing
    cop0->setRegisterHW(CO0_ENTRYLO0, 0x1E);
    // EntryLo1_PFN = 0x1
    // EntryLo1_C = 3 (Cacheable)
    // EntryLo1_D = 1 (Writable)
    // EntryLo1_V = 1 (Valid)
    // EntryLo1_G = 0 (Not global)
    // Remap to pfn 1 for testing
    cop0->setRegisterHW(CO0_ENTRYLO1, 0x5E);
    
    memory->storeWordPhys(0xA0000000, 0x42000002); // tlbwi
    cpu0->stepCPU(1);
    /* End TLBWI */
    
    memory->storeWordPhys(0xA0000004, 0x42000001); // tlbp
    cpu0->stepCPU(1);
    
    ASSERT_EQUAL(0x0040000Au, cop0->getRegister(CO0_ENTRYHI));
    ASSERT_EQUAL(0x1Eu, cop0->getRegister(CO0_ENTRYLO0));
    ASSERT_EQUAL(0x5Eu, cop0->getRegister(CO0_ENTRYLO1));
}

void MIPS_TLBWI() {
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    
    // Set up registers for TLB write
    // Index = 0
    cop0->setRegisterHW(CO0_INDEX, 0);
    // Page Size = 4KB
    // This is pulled directly from Linux:
    //  arch/mips/include/asm/mipsregs.h PM_4K
    // although Pagemask is locked to 0.
    // This is for bits 12..11 and only has an
    // affect if Config3_sp = 1 & PageGrain_esp = 1,
    // aka if support for 1KB pages exists.
    cop0->setRegisterHW(CO0_PAGEMASK, 0x1800);
    // EntryHI_vpn2 = 0x00400000
    cop0->setRegisterHW(CO0_ENTRYHI, 0x00400000);
    // EntryHi_asid = 0xA
    cop0->orRegisterHW(CO0_ENTRYHI, 0x0A);
    // EntryLo0_PFN = 0x0
    // EntryLo0_C = 3 (Cacheable)
    // EntryLo0_D = 1 (Writable)
    // EntryLo0_V = 1 (Valid)
    // EntryLo0_G = 0 (Not global)
    // Remap to pfn 0 for testing
    cop0->setRegisterHW(CO0_ENTRYLO0, 0x1E);
    // EntryLo1_PFN = 0x1
    // EntryLo1_C = 3 (Cacheable)
    // EntryLo1_D = 1 (Writable)
    // EntryLo1_V = 1 (Valid)
    // EntryLo1_G = 0 (Not global)
    // Remap to pfn 1 for testing
    cop0->setRegisterHW(CO0_ENTRYLO1, 0x5E);
    
    memory->storeWordPhys(0xA0000000, 0x42000002); // tlbwi
    cpu0->stepCPU(1);
    
    PMMU::TLBEntry_t entry = memory->getTLBEntry(0, 0);
    
    ASSERT_EQUAL(0x0u, entry.Mask);
    ASSERT_EQUAL(0x00400000u, entry.VPN2);
    ASSERT_EQUAL((uint8_t)0xA, entry.ASID);
    ASSERT_EQUAL(false, entry.G);
    // Entry 0
    ASSERT_EQUAL(0x0u, entry.PFN0 >> 12u);  // 4KB frame
    ASSERT_EQUAL((uint8_t)0x03, entry.C0);
    ASSERT_EQUAL(true, entry.D0);
    ASSERT_EQUAL(true, entry.V0);
    // Entry 1
    ASSERT_EQUAL(0x1u, entry.PFN1 >> 12u);  // 4KB frame
    ASSERT_EQUAL((uint8_t)0x03, entry.C1);
    ASSERT_EQUAL(true, entry.D1);
    ASSERT_EQUAL(true, entry.V1);
}

void MIPS_TLBWR() {
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    
    // Set up registers for TLB write
    
    // Page Size = 4KB
    // This is pulled directly from Linux:
    //  arch/mips/include/asm/mipsregs.h PM_4K
    // although Pagemask is locked to 0.
    // This is for bits 12..11 and only has an
    // affect if Config3_sp = 1 & PageGrain_esp = 1,
    // aka if support for 1KB pages exists.
    cop0->setRegisterHW(CO0_PAGEMASK, 0x1800);
    // EntryHI_vpn2 = 0x00400000
    cop0->setRegisterHW(CO0_ENTRYHI, 0x00400000);
    // EntryHi_asid = 0xA
    cop0->orRegisterHW(CO0_ENTRYHI, 0x0A);
    // EntryLo0_PFN = 0x0
    // EntryLo0_C = 3 (Cacheable)
    // EntryLo0_D = 1 (Writable)
    // EntryLo0_V = 1 (Valid)
    // EntryLo0_G = 0 (Not global)
    // Remap to pfn 0 for testing
    cop0->setRegisterHW(CO0_ENTRYLO0, 0x1E);
    // EntryLo1_PFN = 0x1
    // EntryLo1_C = 3 (Cacheable)
    // EntryLo1_D = 1 (Writable)
    // EntryLo1_V = 1 (Valid)
    // EntryLo1_G = 0 (Not global)
    // Remap to pfn 1 for testing
    cop0->setRegisterHW(CO0_ENTRYLO1, 0x5E);
    
    memory->storeWordPhys(0xA0000000, 0x42000006); // tlbwr
    cpu0->stepCPU(1);
    
    // Get Random index
    uint32_t random = cop0->getRegister(CO0_RANDOM);
    
    PMMU::TLBEntry_t entry = memory->getTLBEntry(0, random);
    
    ASSERT_EQUAL(0x0u, entry.Mask);
    ASSERT_EQUAL(0x00400000u, entry.VPN2);
    ASSERT_EQUAL((uint8_t)0xA, entry.ASID);
    ASSERT_EQUAL(false, entry.G);
    // Entry 0
    ASSERT_EQUAL(0x0u, entry.PFN0 >> 12u);  // 4KB frame
    ASSERT_EQUAL((uint8_t)0x03, entry.C0);
    ASSERT_EQUAL(true, entry.D0);
    ASSERT_EQUAL(true, entry.V0);
    // Entry 1
    ASSERT_EQUAL(0x1u, entry.PFN1 >> 12u);  // 4KB frame
    ASSERT_EQUAL((uint8_t)0x03, entry.C1);
    ASSERT_EQUAL(true, entry.D1);
    ASSERT_EQUAL(true, entry.V1);
}

void MIPS_WAIT() {
    TEST_NOT_IMPLEMENTED();
}

void MIPS_WRPGPR() {
    TEST_NOT_IMPLEMENTED();
}

void TEST_VADDR_TRANSLATION() {
    // Tests translateVaddr
    // Hardly comprehensive but it does test
    // higher size page frame numbers
    // and the dual entry tlb behavior.
    
    // Write Dual TLB entry
    reset();
    cpu0->setPC(0xA0000000);
    Coprocessor0* cop0 = cpu0->getControlCoprocessor();
    
    // Set up registers for TLB write
    // Index = 0
    cop0->setRegisterHW(CO0_INDEX, 0);
    // Page Size = 4KB
    // This is pulled directly from Linux:
    //  arch/mips/include/asm/mipsregs.h PM_4K
    // although Pagemask is locked to 0.
    // This is for bits 12..11 and only has an
    // affect if Config3_sp = 1 & PageGrain_esp = 1,
    // aka if support for 1KB pages exists.
    cop0->setRegisterHW(CO0_PAGEMASK, 0x1800);
    // EntryHI_vpn2 = 0x00400000
    cop0->setRegisterHW(CO0_ENTRYHI, 0x00400000);
    // EntryHi_asid = 0xA
    cop0->orRegisterHW(CO0_ENTRYHI, 0x0A);
    // EntryLo0_PFN = 0x2000
    // EntryLo0_C = 3 (Cacheable)
    // EntryLo0_D = 1 (Writable)
    // EntryLo0_V = 1 (Valid)
    // EntryLo0_G = 0 (Not global)
    // Remap to pfn 2000 for testing
    cop0->setRegisterHW(CO0_ENTRYLO0, 0x8001E);
    // EntryLo1_PFN = 0x2001
    // EntryLo1_C = 3 (Cacheable)
    // EntryLo1_D = 1 (Writable)
    // EntryLo1_V = 1 (Valid)
    // EntryLo1_G = 0 (Not global)
    // Remap to pfn 2001 for testing
    cop0->setRegisterHW(CO0_ENTRYLO1, 0x8005E);
    
    memory->storeWordPhys(0xA0000000, 0x42000002); // tlbwi
    cpu0->stepCPU(1);
    
    cpu0->setPC(0x00400000);
    // PFN 2000
    memory->storeWordPhys(0x02000000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0x02000004, 0x200301f4);   // addi	v1,zero,500
    memory->storeWordPhys(0x02000008, 0x00432025);   // or a0,v0,v1
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0x000001fcu, cpu0->getRegister(4));
    
    // PFN 2001
    cpu0->setPC(0x00401000);
    memory->storeWordPhys(0x02001000, 0x200200c8);   // addi	v0,zero,200
    memory->storeWordPhys(0x02001004, 0x200301f4);   // addi	v1,zero,500
    memory->storeWordPhys(0x02001008, 0x00432026);   // xor a0,v0,v1
    cpu0->stepCPU(3);
    ASSERT_EQUAL(0x0000013cu, cpu0->getRegister(4));
}