//
//  CPU.h
//  MIPS_Emulator
//
//  Created by Matt on 1/6/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef CPU_h
#define CPU_h

#undef DEBUGCPU

#include <cstdint>          // uint32_t
#include <climits>          // CHAR_BIT
#include <thread>           // thread::sleep
#include <sstream>          // For debug printing
#include "ConsoleUI.h"      // stdout access
#include "PMMU.h"           // Memory
#include "Coprocessor0.h"   // Coprocessor0

class MIPSException;        // Exception class forward reference

// MIPS CPU
class CPU {
    public:
    // Interrupt Definitions
    enum MIPSInterrupt {
        SW0 = 0,
        SW1 = 1,
        HW0 = 2,
        HW1 = 3,
        HW2 = 4,
        HW3 = 5,
        HW4 = 6,
        HW5 = 7,
        HALT = 8,
    };
    
    private:
        // Decoder constants
        #define OPCODEMASK  0xFC000000
        #define OPCODESHIFT 26
        #define RSMASK      0x03E00000
        #define RSSHIFT     21
        #define RTMASK      0x001F0000
        #define RTSHIFT     16
        #define RDMASK      0x0000F800
        #define RDSHIFT     11
        #define SHAMTMASK   0x000007C0
        #define SHAMTSHIFT  6
        #define FUNCTMASK   0x0000003F
        #define IMMMASK     0x0000FFFF
        #define JIMMMASK    0x03FFFFFF
        #define SELMASK     0x00000007
        #define COMASK      0x02000000
        #define COSHIFT     25
        #define SCMASK      0x00000020
        #define SCSHIFT     5
    
        // Decoder Macros
        #define DECODE_OPCODE() opcode = (IR & OPCODEMASK) >> OPCODESHIFT
        #define DECODE_RS() rs = (IR & RSMASK) >> RSSHIFT
        #define DECODE_RT() rt = (IR & RTMASK) >> RTSHIFT
        #define DECODE_RD() rd = (IR & RDMASK) >> RDSHIFT
        #define DECODE_SHAMT() shamt = (IR & SHAMTMASK) >> SHAMTSHIFT
        #define DECODE_FUNCT() funct = IR & FUNCTMASK
        #define DECODE_IMM() imm = IR & IMMMASK
        #define DECODE_IMMSE() immse = IR & IMMMASK; immse <<= 16; immse >>= 16
        #define DECODE_JIMM() jimm = IR & JIMMMASK
        #define DECODE_SEL() sel = IR & SELMASK
        #define DECODE_CO() co = (IR & COMASK) >> COSHIFT
        #define DECODE_SC() sc = (IR & SCMASK) >> SCSHIFT
    
        // Dispatch macro
        #ifdef TEST_PROJECT
            #define DISPATCH() return;
        #else
            #define DISPATCH() updateISARep() checkForInts() fetch() DECODE_OPCODE(); goto *opcodeTable[opcode]
        #endif
    
        // Pointer to ConsoleUI for stdout debugging
        ConsoleUI* consoleUI;
    
        // Pointer to the memory manager singleton
        PMMU* memory;
    
        // Control Coprocessor0
        Coprocessor0 cop0;
    
        // GPR Register Set
        uint32_t registers[32];
    
        // Integer Multiply/Divide Registers
        uint32_t HI;
        uint32_t LO;
    
        // Cycle counter
        uint64_t cycleCounter;
    
        // Very temporary thread signal
        volatile uint8_t signal;
    
        // Program Counter and Instruction Register
        uint32_t PC;
        uint32_t IR;
    
        // Branch/delay slot controls
        bool branch;
        bool branchDelay;
        uint32_t branchAddr;
    
        // Currently decoded fields
        uint8_t opcode;
        uint8_t rs;
        uint8_t rt;
        uint8_t rd;
        uint8_t shamt;
        uint8_t funct;
        uint16_t imm;
        int32_t immse;
        uint32_t jimm;
        uint8_t sel;
        uint8_t co;
        uint8_t sc;
    
        // Temporary Variables for dispatchLoop
        // These need to be class members as Clang complains about
        // potentially uninitialized variables due to the heavy use
        // of gotos in dispatchLoop()
        uint64_t tempu64;
        uint32_t tempu32;
        uint32_t tempu32_2;
        uint16_t tempu16;
        uint8_t tempu8;
        int32_t tempi32;
        int64_t tempi64;
        int64_t tempi64_2;
        // Controls behavior of try-catch exception mechanism
        bool exceptRestartLoop;
    
        // Interrupt related stuff
        // Set by an interruptexception()
        // Cleared by eret (which will update CAUSE properly)
        MIPSInterrupt lastReceivedInt;
    
        // String tables for readable instruction decoding
        static const char* opcodeNames[64];
        static const char* functNames[64];
        static const char* special2Names[64];
        static const char* special3Names[64];
        static const char* regimmNames[32];
        static const char* cop0Names[32];
        static const char* cop0CONames[64];
        static const char* registerNames[32];
    
        // CPU Execution Functions
        void decodeAll();
        void dispatchLoop();
        void serviceInterrupt();
    
        // This triggers exceptions for unit testing using
        // a custom instruction.
    #ifdef TEST_PROJECT
        void executeException();
    #endif
    
        // Friendship sadly is not inherited
        // and exceptions necessarily need to modify private members.
        // The alternatives are one super-class with switch-based
        // dispatching to the correct execution handler.
        // or even more code in CPU.cpp.
        friend class MIPSException;
        friend class ColdResetException;
        friend class SoftResetException;
        friend class NonmaskableInterruptException;
        friend class MachineCheckException;
        friend class WatchIFException;
        friend class AddressErrorIFException;
        friend class TLBRefillIFException;
        friend class TLBInvalidIFException;
        friend class TLBExecuteInhibitException;
        friend class CacheErrorIFException;
        friend class BusErrorIFException;
        friend class CoprocessorUnusableException;
        friend class ReservedInstructionException;
        friend class IntegerOverflowException;
        friend class TrapException;
        friend class SystemCallException;
        friend class BreakpointException;
        friend class FloatingPointException;
        friend class Coprocessor2Exception;
        friend class WatchDataException;
        friend class AddressErrorDataException;
        friend class TLBRefillDataException;
        friend class TLBInvalidDataException;
        friend class TLBReadInhibitException;
        friend class TLBModifiedException;
        friend class CacheErrorDataException;
        friend class BusErrorDataException;
        friend class InterruptException;
    
        // Prints out cpu and instruction information
        std::string debugPrint();
    
    public:
        CPU(ConsoleUI* conui, PMMU* memory);
        CPU(PMMU* memory);
    
        // Sets the program counter to an address
        void setPC(uint32_t addr);
    
        // Begins CPU Execution
        void start();
    
        // Sends a signal to the CPU
        bool sendInterrupt(MIPSInterrupt);
        void clearInterrupt(MIPSInterrupt);
    
        // Gets the coprocessor0 object bound to this cpu
        Coprocessor0* getControlCoprocessor();
    
        // For unit testing interface
    #ifdef TEST_PROJECT
        void stepCPU(uint32_t);
        uint32_t getPC();
        uint32_t getIR();
        uint32_t getHI();
        uint32_t getLO();
        uint32_t getRegister(uint8_t);
        uint8_t getOpcode();
        uint8_t getRS();
        uint8_t getRT();
        uint8_t getRD();
        uint8_t getShamt();
        uint8_t getFunct();
        uint16_t getImm();
        int32_t getImmSE();
        uint32_t getJimm();
        uint8_t getSel();
        // Just because these exist doesn't mean they should be used
        void setIR(uint32_t);
        void setHI(uint32_t);
        void setLO(uint32_t);
        void setRegister(uint8_t, uint32_t);
        void setOpcode(uint8_t);
        void setRS(uint8_t);
        void setRT(uint8_t);
        void setRD(uint8_t);
        void setShamt(uint8_t);
        void setFunct(uint8_t);
        void setImm(uint16_t);
        void setImmSE(int32_t);
        void setJimm(uint32_t);
        void setSel(uint8_t);
    #endif
};

#endif /* CPU_h */
