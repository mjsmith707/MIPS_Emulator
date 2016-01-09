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
#include "PMMU.h"           // Memory
#include "Coprocessor0.h"   // Coprocessor0
// MIPS CPU
class CPU {
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
        #define FUNCTSHIFT  0
        #define IMMMASK     0x0000FFFF
        #define IMMSHIFT    0
        #define JIMMMASK    0x03FFFFFF
        #define JIMMSHIFT   0
        #define SELMASK     0x7
    
        // Decoder Macros
        #define DECODE_OPCODE() opcode = (IR & OPCODEMASK) >> OPCODESHIFT
        #define DECODE_RS() rs = (IR & RSMASK) >> RSSHIFT
        #define DECODE_RT() rt = (IR & RTMASK) >> RTSHIFT
        #define DECODE_RD() rd = (IR & RDMASK) >> RDSHIFT
        #define DECODE_SHAMT() shamt = (IR & SHAMTMASK) >> SHAMTSHIFT
        #define DECODE_FUNCT() funct = (IR & FUNCTMASK) >> FUNCTSHIFT
        #define DECODE_IMM() imm = (IR & IMMMASK) >> IMMSHIFT
        #define DECODE_JIMM() jimm = (IR & JIMMMASK) >> JIMMSHIFT
        #define DECODE_SEL() DECODE_IMM(); sel = imm & SELMASK
    
        // Pointer to the memory manager singleton
        PMMU* memory;
    
        // Control Coprocessor0
        Coprocessor0 cop0_processor;
    
        // GPR Register Set
        uint32_t registers[32];
    
        // Integer Multiply/Divide Registers
        uint32_t HI;
        uint32_t LO;
    
        // Cycle counter
        uint64_t cycleCounter;
    
        // Very temporary thread signal
        uint32_t signal;
    
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
        uint32_t jimm;
        uint8_t sel;
    
        // Temporary Variables for dispatchLoop
        // These need to be class members as Clang complains about
        // potentially uninitialized variables due to the heavy use
        // of gotos in dispatchLoop()
        uint64_t tempu64;
        uint32_t tempu32;
        int32_t tempi32;
    
        // String tables for readable instruction decoding
        static const char* opcodeNames[64];
        static const char* functNames[64];
        static const char* special2Names[64];
        static const char* special3Names[64];
        static const char* regimmNames[32];
        static const char* cop0Names[32];
        static const char* registerNames[32];
    
        // CPU Execution Functions
        //void fetch();
        void decodeAll();
        void dispatchLoop();
    
        // Prints out cpu and instruction information
        void debugPrint();
    
    public:
        CPU(PMMU* memory);
    
        // Sets the program counter to an address
        void setPC(uint32_t addr);
    
        // Begins CPU Execution
        void start();
    
        // Sends a signal to the CPU
        void sendSignal(uint32_t);
};

#endif /* CPU_h */
