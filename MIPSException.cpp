//
//  MIPSException.cpp
//  MIPS_Emulator
//
//  Created by Matt on 2/26/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "MIPSException.h"

// General Exception Processing (pg 88 vol 3)
void MIPSException::generalException(CPU* cpu, ExceptionType etype, ExceptionCode ecode) {
    uint32_t vectorOffset = 0;
    uint32_t vectorBase = 0;
    // If status_exl == 1
    if ((cpu->cop0.getRegister(CO0_STATUS) & STATUS_EXL) > 0) {
        vectorOffset = 0x180;
    }
    else {
        if (cpu->branchDelay) {
            // EPC = branch/jump instruction
            cpu->cop0.setRegisterHW(CO0_EPC, cpu->PC-8);
            // Cause_bd = 1
            cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) | CAUSE_BD);
        }
        else {
            // EPC = instruction
            cpu->cop0.setRegisterHW(CO0_EPC, cpu->PC-4);
            // Cause_bd = 0
            cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) & ~(CAUSE_BD));
        }
        
        // TODO: NewShadowSt = SRSCtl_ess Not Implemented
        if (etype == ExceptionType::TLBRefill) {
            vectorOffset = 0x000;
        }
        else if (etype == ExceptionType::Interrupt) {
            // If Cause_iv = 0
            if ((cpu->cop0.getRegister(CO0_CAUSE) & CAUSE_IV) == 0) {
                vectorOffset = 0x180;
            }
            else {
                // If Status_bev = 1 or IntCtl_vs = 0)
                if (((cpu->cop0.getRegister(CO0_STATUS) & STATUS_BEV) > 0) || ((cpu->cop0.getRegister(CO0_INTCTL) & INTCTL_VS) == 0)) {
                    vectorOffset = 0x200;
                }
                else {
                    // If Config3_veic = 1
                    if ((cpu->cop0.getRegister(CO0_CONFIG3) & CONFIG3_VEIC) > 0) {
                        // TODO: Vectored Interrupt Support
                    }
                    else {
                        // TODO: Vectored Interrupt Support
                    }
                    // If EIC_option3 TODO: Vectored Interrupt Support
                }
            }
        }
    }
    // FIXME: Cause_ce = FaultingCoprocessorNumber but we only have 0 so far
    // Cause_ce = 0
    cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) & ~(CAUSE_CE));
    
    // Set Cause_exccode
    uint32_t ecodeu = static_cast<uint32_t>(ecode);
    cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) & ~(CAUSE_EXCCODE));
    cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) | (ecodeu << 2));
    
    // Status_exl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_EXL);
    
    // If Status_bev = 1
    if ((cpu->cop0.getRegister(CO0_STATUS) & STATUS_BEV) > 0) {
        vectorBase = 0xBFC00200;
    }
    else {
        // Archrev >= 2
        vectorBase = cpu->cop0.getRegister(CO0_EBASE) & EBASE_EBASEFULL;
    }
    
    // Set PC
    cpu->PC = vectorBase & 0xC0000000;
    vectorBase &= ~(0xC0000000);
    vectorOffset &= ~(0xC0000000);
    vectorBase += vectorOffset;
    cpu->PC |= (vectorBase & ~(0xC0000000));
}

// Common functions for many exceptions
// Sets BadVaddr to the failing address
void MIPSException::setBadVaddr(CPU* cpu) {
    // Set BadVAddr
    cpu->cop0.setRegisterHW(CO0_BADVADDR, cpu->PC-4);
}

// Sets Context based on Config3_ctxtc
void MIPSException::setContextBadVPN2(CPU* cpu) {
    // If Config3_ctxtc > 0
    if ((cpu->cop0.getRegister(CO0_CONFIG3) & CONFIG3_CTXTC) > 0) {
        // ContextConfig optional so not implemented
    }
    else {
        // Context_vpn2 contains VA31-13
        cpu->cop0.setRegisterHW(CO0_CONTEXT, cpu->cop0.getRegister(CO0_CONTEXT) & ~(CONTEXT_BADVPN2));
        uint32_t va = cpu->PC-4;
        va >>= 18;
        va <<= 4;
        cpu->cop0.setRegisterHW(CO0_CONTEXT, cpu->cop0.getRegister(CO0_CONTEXT) | va);
    }
}

// Sets EntryHi_vpn2 to failing VAddr
void MIPSException::setEntryHiVA(CPU* cpu) {
    // EntryHi_vpn2 = va_31-13
    cpu->cop0.setRegisterHW(CO0_ENTRYHI, cpu->cop0.getRegister(CO0_ENTRYHI) & ~(ENTRYHI_VPN2));
    uint32_t va = cpu->PC-4;
    va >>= 18;
    va <<= 13;
    cpu->cop0.setRegisterHW(CO0_ENTRYHI, cpu->cop0.getRegister(CO0_ENTRYHI) | va);
    // FIXME: EntryHi_asid = asid referenced (TLB related)
}

// Sets ErrorEPC to the restart location
void MIPSException::setErrorEPC(CPU* cpu) {
    // Set ErrorEPC
    if (cpu->branchDelay) {
        // Set to branch/jump instruction address
        cpu->cop0.setRegisterHW(CO0_ERROREPC, cpu->PC-8);
        cpu->branchDelay = false;
    }
    else {
        // Set to instruction address
        cpu->cop0.setRegisterHW(CO0_ERROREPC, cpu->PC-4);
    }
}

// Sets Status_exl to 1 for general exceptions
void MIPSException::setExlOn(CPU* cpu) {
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_EXL);
}

// Cold Reset Exception
ColdResetException::ColdResetException() {
}

void ColdResetException::execute(CPU* cpu) {
    // TODO: Random = TLBEntries -1
    // PageMask_maskX = 0
    cpu->cop0.setRegisterHW(CO0_PAGEMASK, cpu->cop0.getRegister(CO0_PAGEMASK) & ~(PAGEMASK_MASKX));
    // PageGrain_esp = 0
    cpu->cop0.setRegisterHW(CO0_PAGEGRAIN, cpu->cop0.getRegister(CO0_PAGEGRAIN) & ~(PAGEGRAIN_ESP));
    // Wired = 0
    cpu->cop0.setRegisterHW(CO0_WIRED, 0);
    // HWREna = 0
    cpu->cop0.setRegisterHW(CO0_HWRENA, 0);
    // EntryHi_vpn2x = 0
    cpu->cop0.setRegisterHW(CO0_ENTRYHI, cpu->cop0.getRegister(CO0_ENTRYHI) & ~(ENTRYHI_VPN2X));
    // Status_rp = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_RP));
    // Status_bev = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_BEV);
    // Status_ts = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_TS));
    // Status_sr = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_SR));
    // Status_nmi = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_NMI));
    // Status_erl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_ERL);
    // IntCtl_vs = 0
    cpu->cop0.setRegisterHW(CO0_INTCTL, cpu->cop0.getRegister(CO0_INTCTL) & ~(INTCTL_VS));
    // SRSCtl_hss = HighestImplementedShadowSet = 0
    cpu->cop0.setRegisterHW(CO0_SRSCTL, cpu->cop0.getRegister(CO0_SRSCTL) & ~(SRSCTL_HSS));
    // SRSCtl_ess = 0
    cpu->cop0.setRegisterHW(CO0_SRSCTL, cpu->cop0.getRegister(CO0_SRSCTL) & ~(SRSCTL_ESS));
    // SRSCtl_pss = 0
    cpu->cop0.setRegisterHW(CO0_SRSCTL, cpu->cop0.getRegister(CO0_SRSCTL) & ~(SRSCTL_PSS));
    // SRSCtl_css = 0
    cpu->cop0.setRegisterHW(CO0_SRSCTL, cpu->cop0.getRegister(CO0_SRSCTL) & ~(SRSCTL_CSS));
    // TODO: SRSMap = 0 Not Implemented
    // Cause_dc = 0
    cpu->cop0.setRegisterHW(CO0_CAUSE, cpu->cop0.getRegister(CO0_CAUSE) & ~(CAUSE_DC));
    // EBase_exceptionbase = 0
    cpu->cop0.setRegisterHW(CO0_EBASE, cpu->cop0.getRegister(CO0_EBASE) & ~(EBASE_EBASE));
    // Config_k0 = 2 Suggested
    cpu->cop0.setRegisterHW(CO0_CONFIG0, cpu->cop0.getRegister(CO0_CONFIG0) & ~(CONFIG0_K0));
    cpu->cop0.setRegisterHW(CO0_CONFIG0, cpu->cop0.getRegister(CO0_CONFIG0) | 0x2);
    // Config1 = ConfigurationState
    cpu->cop0.resetRegister(CO0_CONFIG1);
    // Config2 = ConfigurationState
    cpu->cop0.resetRegister(CO0_CONFIG2);
    // Config3 = ConfigurationState
    cpu->cop0.resetRegister(CO0_CONFIG3);
    // TODO: WatchLo[n]_i = 0 Not Implemented
    // TODO: WatchLo[n]_r = 0 Not Implemented
    // TODO: WatchLo[n]_w = 0 Not Implemented
    // TODO: PerfCnt.Control[n]_ie = 0 Not Implemented

    setErrorEPC(cpu);
    
    // Set Program Counter
    cpu->PC = 0xBFC00000;
}

// Cold Reset Exception
SoftResetException::SoftResetException() {
}

void SoftResetException::execute(CPU* cpu) {
    // PageMask_maskX = 0
    cpu->cop0.setRegisterHW(CO0_PAGEMASK, cpu->cop0.getRegister(CO0_PAGEMASK) & ~(PAGEMASK_MASKX));
    // PageGrain_esp = 0
    cpu->cop0.setRegisterHW(CO0_PAGEGRAIN, cpu->cop0.getRegister(CO0_PAGEGRAIN) & ~(PAGEGRAIN_ESP));
    // EntryHi_vpn2x = 0
    cpu->cop0.setRegisterHW(CO0_ENTRYHI, cpu->cop0.getRegister(CO0_ENTRYHI) & ~(ENTRYHI_VPN2X));
    // Config_k0 = 2
    cpu->cop0.setRegisterHW(CO0_CONFIG0, cpu->cop0.getRegister(CO0_CONFIG0) & ~(CONFIG0_K0));
    cpu->cop0.setRegisterHW(CO0_CONFIG0, cpu->cop0.getRegister(CO0_CONFIG0) | 0x2);
    // Status_rp = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_RP));
    // Status_bev = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_BEV);
    // Status_ts = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_TS));
    // Status_sr = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_SR);
    // Status_nmi = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_NMI));
    // Status_erl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_ERL);
    // TODO: WatchLo[n]_i = 0 Not Implemented
    // TODO: WatchLo[n]_r = 0 Not Implemented
    // TODO: WatchLo[n]_w = 0 Not Implemented
    // TODO: PerfCnt.Control[n]_ie = 0 Not Implemented
    
    setErrorEPC(cpu);
    
    // Set Program Counter
    cpu->PC = 0xBFC00000;
}

// Nonmaskable Interrupt Exception
NonmaskableInterruptException::NonmaskableInterruptException() {
}

void NonmaskableInterruptException::execute(CPU* cpu) {
    // Status_bev = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_BEV);
    // Status_ts = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_TS));
    // Status_sr = 0
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_SR);
    // Status_nmi = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_NMI);
    // Status_erl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_ERL);
    
    setErrorEPC(cpu);
    
    // Set Program Counter
    cpu->PC = 0xBFC00000;
}

// Machine Check Exception
MachineCheckException::MachineCheckException() {
}

void MachineCheckException::execute(CPU* cpu) {
    // FIXME: Some vague descriptions of other possible saved state (pg 94 vol3)
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::MCheck);
}

// Watch - Instruction Fetch Exception
WatchIFException::WatchIFException() {
}

void WatchIFException::execute(CPU* cpu) {
    // FIXME: Cause_wp set for something idk
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::WATCH);
}

// Address Error - Instruction Fetch Exception
AddressErrorIFException::AddressErrorIFException() {
}

void AddressErrorIFException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::AdEL);
}

// TLB Refill - Instruction Fetch Exception
TLBRefillIFException::TLBRefillIFException() {
}

void TLBRefillIFException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    generalException(cpu, ExceptionType::TLBRefill, ExceptionCode::TLBL);
}

// TLB Invalid - Instruction Fetch Exception
TLBInvalidIFException::TLBInvalidIFException() {
}

void TLBInvalidIFException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::TLBL);
}

// TLB Execute Inhibit Exception
TLBExecuteInhibitException::TLBExecuteInhibitException() {
}

void TLBExecuteInhibitException::execute(CPU* cpu) {
    // If PageGrain_iec = 1 then TLBXI else TLBL
    ExceptionCode code = (cpu->cop0.getRegister(CO0_PAGEGRAIN) & PAGEGRAIN_IEC) > 0 ? ExceptionCode::TLBXI : ExceptionCode::TLBL;
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, code);
}

// Cache Error - Instruction Fetch Exception
// Not even sure if i need this really..
CacheErrorIFException::CacheErrorIFException() {
}

void CacheErrorIFException::execute(CPU* cpu) {
    // TODO: Set CacheErr, Register is optional
    
    // Status_erl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_ERL);
    setErrorEPC(cpu);
    
    // If Status_bev = 1
    if ((cpu->cop0.getRegister(CO0_STATUS) & STATUS_BEV) > 0) {
        cpu->PC = 0xBFC00200 + 0x100;
    }
    else {
        // Archrev >= 2
        // This makes zero sense but ok
        cpu->PC = cpu->cop0.getRegister(CO0_EBASE);
        cpu->PC |= 0x20000100;
        cpu->PC &= 0xFFFFFF00;
    }
}

// Bus Error - Instruction Fetch Exception
BusErrorIFException::BusErrorIFException() {
}

void BusErrorIFException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::IBE);
}

// Coprocessor Unusable Exception
CoprocessorUnusableException::CoprocessorUnusableException() {
}

void CoprocessorUnusableException::execute(CPU* cpu) {
    // FIXME: Cause_ce is described in generalException
    // And also described as 'additional' here
    // So which one does it...
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::CpU);
}

// Reserved Instruction Exception
ReservedInstructionException::ReservedInstructionException() {
}

void ReservedInstructionException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::RI);
}

// Integer Overflow Exception
IntegerOverflowException::IntegerOverflowException() {
}

void IntegerOverflowException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::Ov);
}

// Trap Exception
TrapException::TrapException() {
}

void TrapException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::Tr);
}

// System Call Exception
SystemCallException::SystemCallException() {
}

void SystemCallException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::Sys);
}

// Breakpoint Exception
BreakpointException::BreakpointException() {
}

void BreakpointException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::Bp);
}

// Floating-point Exception
FloatingPointException::FloatingPointException() {
}

void FloatingPointException::execute(CPU* cpu) {
    // TODO: Save cause to FCSR
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::FPE);
}

// Coprocessor 2 Exception
Coprocessor2Exception::Coprocessor2Exception() {
}

void Coprocessor2Exception::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::C2E);
}

// Watch - Data Exception
WatchDataException::WatchDataException() {
}

void WatchDataException::execute(CPU* cpu) {
    // FIXME: Cause_wp set for something idk
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::WATCH);
}

// Address Error - Data Exception
AddressErrorDataException::AddressErrorDataException() {
}

void AddressErrorDataException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::AdES);
}

// TLB Refill - Data Exception
TLBRefillDataException::TLBRefillDataException() {
}

void TLBRefillDataException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    generalException(cpu, ExceptionType::TLBRefill, ExceptionCode::TLBS);
}

// TLB Invalid - Data Exception
TLBInvalidDataException::TLBInvalidDataException() {
}

void TLBInvalidDataException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::TLBS);
}

// TLB Read-Inhibit Exception
TLBReadInhibitException::TLBReadInhibitException() {
}

void TLBReadInhibitException::execute(CPU* cpu) {
    // If PageGrain_iec = 1 then TLBRI else TLBL
    ExceptionCode code = (cpu->cop0.getRegister(CO0_PAGEGRAIN) & PAGEGRAIN_IEC) > 0 ? ExceptionCode::TLBRI : ExceptionCode::TLBL;
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, code);
}

// TLB Modified - Data Exception
TLBModifiedException::TLBModifiedException() {
}

void TLBModifiedException::execute(CPU* cpu) {
    setBadVaddr(cpu);
    setContextBadVPN2(cpu);
    setEntryHiVA(cpu);
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::Mod);
}

// Cache Error - Data Exception
CacheErrorDataException::CacheErrorDataException() {
}

void CacheErrorDataException::execute(CPU* cpu) {
    // TODO: Set CacheErr, Register is optional
    
    // Status_erl = 1
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) | STATUS_ERL);
    setErrorEPC(cpu);
    
    // If Status_bev = 1
    if ((cpu->cop0.getRegister(CO0_STATUS) & STATUS_BEV) > 0) {
        cpu->PC = 0xBFC00200 + 0x100;
    }
    else {
        // Archrev >= 2
        // This makes zero sense but ok
        cpu->PC = cpu->cop0.getRegister(CO0_EBASE);
        cpu->PC |= 0x20000100;
        cpu->PC &= 0xFFFFFF00;
    }
}

// Bus Error - Data Exception
BusErrorDataException::BusErrorDataException() {
}

void BusErrorDataException::execute(CPU* cpu) {
    setExlOn(cpu);
    generalException(cpu, ExceptionType::None, ExceptionCode::DBE);
}

// Interrupt Exception
InterruptException::InterruptException() {
}

void InterruptException::execute(CPU* cpu) {
    // FIXME: Not sure if this is correct but it seems like it
    cpu->cop0.setRegisterHW(CO0_STATUS, cpu->cop0.getRegister(CO0_STATUS) & ~(STATUS_EXL));
    generalException(cpu, ExceptionType::Interrupt, ExceptionCode::Int);
}