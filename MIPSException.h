//
//  MIPSException.h
//  MIPS_Emulator
//
//  Created by Matt on 2/26/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef MIPSException_h
#define MIPSException_h

#include "CPU.h"

// Base Exception Class
class MIPSException {
    protected:
        // Exception Type
        enum class ExceptionType {
            TLBRefill,
            Interrupt,
            None
        };
        
        // Exception Code (Table 9.40 pg 189 vol3)
        enum ExceptionCode {
            Int =   0x00,
            Mod =   0x01,
            TLBL =  0x02,
            TLBS =  0x03,
            AdEL =  0x04,
            AdES =  0x05,
            IBE =   0x06,
            DBE =   0x07,
            Sys =   0x08,
            Bp =    0x09,
            RI =    0x0a,
            CpU =   0x0b,
            Ov =    0x0c,
            Tr =    0x0d,
            MSAFPE = 0x0e,
            FPE =   0x0f,
            C2E =   0x12,
            TLBRI = 0x13,
            TLBXI = 0x14,
            MSADis = 0x15,
            MDMX =  0x16,
            WATCH = 0x17,
            MCheck = 0x18,
            Thread = 0x19,
            DSPDis = 0x1a,
            GE =    0x1b,
            CacheErr = 0x1e,
        };
    
        void generalException(CPU*, ExceptionType, ExceptionCode);
        void setBadVaddr(CPU*);
        void setContextBadVPN2(CPU*);
        void setEntryHiVA(CPU*);
        void setErrorEPC(CPU*);
        void setExlOn(CPU* cpu);
    
    public:
        virtual void execute(CPU*) = 0;
};

// Cold Reset NMI Exception
class ColdResetException : public MIPSException {
    public:
        ColdResetException();
        void execute(CPU*);
};

// Soft Reset NMI Exception
class SoftResetException : public MIPSException {
public:
    SoftResetException();
    void execute(CPU*);
};

// Nonmaskable Interrupt Exception
class NonmaskableInterruptException : public MIPSException {
public:
    NonmaskableInterruptException();
    void execute(CPU*);
};

// Machine Check Exception
class MachineCheckException : public MIPSException {
public:
    MachineCheckException();
    void execute(CPU*);
};

// Watch - Instruction Fetch Exception
class WatchIFException : public MIPSException {
public:
    WatchIFException();
    void execute(CPU*);
};

// Address Error - Instruction Fetch Exception
class AddressErrorIFException : public MIPSException {
public:
    AddressErrorIFException();
    void execute(CPU*);
};

// TLB Refill - Instruction Fetch Exception
class TLBRefillIFException : public MIPSException {
public:
    TLBRefillIFException();
    void execute(CPU*);
};

// TLB Invalid - Instruction Fetch Exception
class TLBInvalidIFException : public MIPSException {
public:
    TLBInvalidIFException();
    void execute(CPU*);
};

// TLB Execute-Inhibit Exception
class TLBExecuteInhibitException : public MIPSException {
public:
    TLBExecuteInhibitException();
    void execute(CPU*);
};

// Cache Error - Instruction Fetch Exception
class CacheErrorIFException : public MIPSException {
public:
    CacheErrorIFException();
    void execute(CPU*);
};

// Bus Error - Instruction Fetch Exception
class BusErrorIFException : public MIPSException {
public:
    BusErrorIFException();
    void execute(CPU*);
};

// Coprocessor Unusable Exception
class CoprocessorUnusableException : public MIPSException {
public:
    CoprocessorUnusableException();
    void execute(CPU*);
};

// Reserved Instruction Exception
class ReservedInstructionException : public MIPSException {
public:
    ReservedInstructionException();
    void execute(CPU*);
};

// Integer Overflow Exception
class IntegerOverflowException : public MIPSException {
public:
    IntegerOverflowException();
    void execute(CPU*);
};

// Trap Exception
class TrapException : public MIPSException {
public:
    TrapException();
    void execute(CPU*);
};

// System Call Exception
class SystemCallException : public MIPSException {
public:
    SystemCallException();
    void execute(CPU*);
};

// Breakpoint Call Exception
class BreakpointException : public MIPSException {
public:
    BreakpointException();
    void execute(CPU*);
};

// Floating-point Exception
class FloatingPointException : public MIPSException {
public:
    FloatingPointException();
    void execute(CPU*);
};

// Coprocessor 2 Exception
class Coprocessor2Exception : public MIPSException {
public:
    Coprocessor2Exception();
    void execute(CPU*);
};

// Watch - Data Exception
class WatchDataException : public MIPSException {
public:
    WatchDataException();
    void execute(CPU*);
};

// Address Error - Data Exception
class AddressErrorDataException : public MIPSException {
public:
    AddressErrorDataException();
    void execute(CPU*);
};

// TLB Refill - Data Exception
class TLBRefillDataException : public MIPSException {
public:
    TLBRefillDataException();
    void execute(CPU*);
};

// TLB Invalid - Data Exception
class TLBInvalidDataException : public MIPSException {
public:
    TLBInvalidDataException();
    void execute(CPU*);
};

// TLB Read-Inhibit
class TLBReadInhibitException : public MIPSException {
public:
    TLBReadInhibitException();
    void execute(CPU*);
};

// TLB Modified - Data Exception
class TLBModifiedException : public MIPSException {
public:
    TLBModifiedException();
    void execute(CPU*);
};

// Cache Error - Data Exception
class CacheErrorDataException : public MIPSException {
public:
    CacheErrorDataException();
    void execute(CPU*);
};

// Bus Error - Data Exception
class BusErrorDataException : public MIPSException {
public:
    BusErrorDataException();
    void execute(CPU*);
};

// Interrupt Exception
class InterruptException : public MIPSException {
public:
    InterruptException();
    void execute(CPU*);
};

#endif /* MIPSException_h */
