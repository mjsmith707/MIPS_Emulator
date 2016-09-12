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
#include <chrono>           // For high_resolution_timer and others
#include "ConsoleUI.h"      // stdout access
#include "PMMU.h"           // Memory
#include "Coprocessor0.h"   // Coprocessor0
#include "Clockable_Device.h"   // Interface for clock driven devices
#include <fstream>

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
    };
    
    // Thread signaling Definitions
    enum CPUSignal {
        CONTINUE = 0,
        BREAKPTACTIVE = 1,
        PAUSE = 2,
        STEP = 3,
        HALT = 4
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
            #define DISPATCH() updateISARep() checkCycle() checkSignal() checkCountComp() checkClockable() checkForInts() fetch() DECODE_OPCODE(); goto *opcodeTable[opcode]
        #endif
    
        // CPU Number
        const uint8_t CPUNUM;
    
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
        uint64_t cycleLimit;
    
        // Shadow copies of count/compare
        uint32_t count;
        uint32_t compare;
    
        // Thread signaling variable
        volatile CPUSignal signal;
    
        // Program Counter and Instruction Register
        uint32_t PC;
        uint32_t IR;
    
        // Branch/delay slot controls
        bool branch;
        bool branchDelay;
        uint32_t branchAddr;
    
        // Atomic LL/SC
        // Page frame number of current RWM sequence
        uint32_t LLPFN;
        bool LLBit;
    
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
    
        // Timing variables
        std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
    
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
    
        // Breakpoint related
        volatile bool singleStep;
        volatile uint32_t breakpointTarget;
    
        // String tables for readable instruction decoding
        static const char* opcodeNames[64];
        static const char* functNames[64];
        static const char* special2Names[64];
        static const char* special3Names[64];
        static const char* regimmNames[32];
        static const char* cop0Names[32];
        static const char* cop0CONames[64];
        static const char* registerNames[32];
    
        // Clock driven device stuff
        #define CLOCKDEV_MAX 32
        uint32_t clockDevSize;
        typedef struct ClockDev_t {
            uint32_t interval;
            Clockable_Device* dev;
            ClockDev_t() : interval(0), dev(nullptr) {}
            ClockDev_t(uint32_t interval, Clockable_Device* dev) : interval(interval), dev(dev) {}
        } ClockDev_t;
        ClockDev_t clockableDevices[CLOCKDEV_MAX];
    
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
    
    public:
        CPU(uint8_t cpuNum, ConsoleUI* conui, PMMU* memory);
        CPU(uint8_t cpuNum, PMMU* memory);
    
        // Sets the program counter to an address
        void setPC(uint32_t addr);
    
        // Sets a register to a value
        void setRegister(uint8_t, uint32_t);
    
        // Sets a breakpoint target
        void setBreakpoint(uint32_t);
    
        // Begins CPU Execution
        void start();
    
        // Sends an interrupt to the CPU
        bool sendInterrupt(MIPSInterrupt);
        // Clears an interrupt from the CPU
        void clearInterrupt(MIPSInterrupt);
    
        // Sends a signal to the CPU thread
        void sendThreadSignal(CPUSignal);
    
        // Gets the current thread state
        CPUSignal getThreadSignal();
    
        // Prints out cpu and instruction information
        std::string debugPrint();
    
        // Prints out GPR information
        std::string printRegisters();
    
        // Gets the coprocessor0 object bound to this cpu
        Coprocessor0* getControlCoprocessor();
    
        // Gets the CPU Number
        uint8_t getCPUNum();
    
        // Attaches a clockable_device type to the CPU
        void attachClockableDevice(Clockable_Device*);
    
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
        bool getLLBit();
        uint32_t getLLPFN();
        // Just because these exist doesn't mean they should be used
        void setIR(uint32_t);
        void setHI(uint32_t);
        void setLO(uint32_t);
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
        void setLLBit(bool);
        void setLLPFN(uint32_t);
    #endif
};

#endif /* CPU_h */
