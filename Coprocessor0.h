//
//  Coprocessor0.h
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef Coprocessor0_h
#define Coprocessor0_h

#include "COP0Register.h"
#include <thread>
#include <atomic>
#include <mutex>

// Register Name Macros (number,sel)
#define CO0_INDEX       0,0
#define CO0_RANDOM      1,0
#define CO0_ENTRYLO0    2,0
#define CO0_ENTRYLO1    3,0
#define CO0_CONTEXT     4,0
#define CO0_PAGEMASK    5,0
#define CO0_PAGEGRAIN   5,1
#define CO0_WIRED       6,0
#define CO0_HWRENA      7,0
#define CO0_BADVADDR    8,0
#define CO0_COUNT       9,0
#define CO0_ENTRYHI     10,0
#define CO0_COMPARE     11,0
#define CO0_STATUS      12,0
#define CO0_INTCTL      12,1
#define CO0_SRSCTL      12,2
#define CO0_SRSMAP      12,3
#define CO0_CAUSE       13,0
#define CO0_EPC         14,0
#define CO0_PRID        15,0
#define CO0_EBASE       15,1
#define CO0_CONFIG0     16,0
#define CO0_CONFIG1     16,1
#define CO0_CONFIG2     16,2
#define CO0_CONFIG3     16,3
#define CO0_LLADDR      17,0
#define CO0_WATCHLO     18,0
#define CO0_WATCHHI     19,0
#define CO0_XCONTEXT    20,0
#define CO0_TAGLO       28,0
#define CO0_TAGHI       29,0
#define CO0_ERROREPC    30,0

// Index Register Fields (CP0 Register 0, Select 0)
#define INDEX_PBIT      0x80000000

// Context Register Fields (CP0 Register 4, Select 0)
#define CONTEXT_PTEBASE 0xFF800000
#define CONTEXT_BADVPN2 0x007FFFF0

// PageMask Register Fields (CP0 Register 5, Select 0)
#define PAGEMASK_MASK   0x1FFFE000
#define PAGEMASK_MASKX  0x00001800

// PageGrain Register Fields (CP0 Register 5, Select 1)
#define PAGEGRAIN_RIE   0x80000000
#define PAGEGRAIN_XIE   0x40000000
#define PAGEGRAIN_ELPA  0x20000000
#define PAGEGRAIN_ESP   0x10000000
#define PAGEGRAIN_IEC   0x08000000
#define PAGEGRAIN_S32   0x04000000
#define PAGEGRAIN_ASE   0x00001F00
#define PAGEGRAIN_MCCAUSE   0x0000001F

// EntryHi Register Fields (CP0 Register 10, Select 0)
#define ENTRYHI_VPN2    0xFFFFE000
#define ENTRYHI_VPN2X   0x00001800
#define ENTRYHI_EH      0x00000400
#define ENTRYHI_INV     0x00000400
#define ENTRYHI_ASIDX   0x00000300
#define ENTRYHI_ASID    0x000000FF

// Status Register Fields (CP0 Register 12, Select 0)
#define STATUS_CU3      0x80000000
#define STATUS_CU2      0x40000000
#define STATUS_CU1      0x20000000
#define STATUS_CU0      0x10000000
#define STATUS_RP       0x08000000
#define STATUS_FR       0x04000000
#define STATUS_RE       0x02000000
#define STATUS_MX       0x01000000
#define STATUS_PX       0x00800000
#define STATUS_BEV      0x00400000
#define STATUS_TS       0x00200000
#define STATUS_SR       0x00100000
#define STATUS_NMI      0x00080000
#define STATUS_IMPL1    0x00020000
#define STATUS_IMPL0    0x00010000
#define STATUS_IM7      0x00008000
#define STATUS_IM6      0x00004000
#define STATUS_IM5      0x00002000
#define STATUS_IM4      0x00001000
#define STATUS_IM3      0x00000800
#define STATUS_IM2      0x00000400
#define STATUS_IM1      0x00000200
#define STATUS_IM0      0x00000100
#define STATUS_KX       0x00000080
#define STATUS_SX       0x00000040
#define STATUS_UX       0x00000020
#define STATUS_KSU      0x00000018
#define STATUS_UM       0x00000010
#define STATUS_R0       0x00000008
#define STATUS_ERL      0x00000004
#define STATUS_EXL      0x00000002
#define STATUS_IE       0x00000001
#define STATUS_INTS     0x00000007

// IntCtl Register Fields (CP0 Register 12, Select 1)
#define INTCTL_IPTI     0xE0000000
#define INTCTL_IPPCI    0x1C000000
#define INTCTL_IPFDC    0x03800000
#define INTCTL_MCUASE   0x007FC000
#define INTCTL_VS       0x000003E0

// SRSCtl Register (CP0 Register 12, Select 2)
#define SRSCTL_HSS      0x3C000000
#define SRSCTL_EICSS    0x003C0000
#define SRSCTL_ESS      0x0000F000
#define SRSCTL_PSS      0x000003C0
#define SRSCTL_CSS      0x0000000F

// Cause Register (CP0 Register 13, Select 0)
#define CAUSE_BD        0x80000000
#define CAUSE_TI        0x40000000
#define CAUSE_CE        0x30000000
#define CAUSE_DC        0x08000000
#define CAUSE_PCI       0x04000000
#define CAUSE_ASE1      0x03000000
#define CAUSE_IV        0x00800000
#define CAUSE_WP        0x00400000
#define CAUSE_FDCI      0x00200000
#define CAUSE_ASE2      0x00030000
#define CAUSE_RIPL      0x0000FC00
#define CAUSE_IP7       0x00008000
#define CAUSE_IP6       0x00004000
#define CAUSE_IP5       0x00002000
#define CAUSE_IP4       0x00001000
#define CAUSE_IP3       0x00000800
#define CAUSE_IP2       0x00000400
#define CAUSE_IP1       0x00000200
#define CAUSE_IP0       0x00000100
#define CAUSE_EXCCODE   0x0000007C

// EBase Register (CP0 Register 15, Select 1)
#define EBASE_EBASEFULL 0xFFFFF000
#define EBASE_EBASE     0x3FFFF000
#define EBASE_CPUNUM    0x000003FF

// Config Register (CP0 Register 16, Select 0)
#define CONFIG0_M       0x80000000
#define CONFIG0_K23     0x70000000
#define CONFIG0_KU      0x0E000000
#define CONFIG0_IMPL    0x01FF0000
#define CONFIG0_BE      0x00008000
#define CONFIG0_AT      0x00006000
#define CONFIG0_AR      0x00001C00
#define CONFIG0_MT      0x00000380
#define CONFIG0_VI      0x00000008
#define CONFIG0_K0      0x00000007

// Config3 Register (CP0 Register 16, Select 3)
#define CONFIG3_M       0x80000000
#define CONFIG3_CMGCR   0x20000000
#define CONFIG3_MSAP    0x10000000
#define CONFIG3_BP      0x08000000
#define CONFIG3_BI      0x04000000
#define CONFIG3_SC      0x02000000
#define CONFIG3_PW      0x01000000
#define CONFIG3_VZ      0x00800000
#define CONFIG3_IPLW    0x00600000
#define CONFIG3_MMAR    0x001C0000
#define CONFIG3_MUCON   0x00020000
#define CONFIG3_ISAONEXEC   0x00010000
#define CONFIG3_ISA     0x0000C000
#define CONFIG3_ULRI    0x00002000
#define CONFIG3_RXI     0x00001000
#define CONFIG3_DSP2P   0x00000800
#define CONFIG3_DSPP    0x00000400
#define CONFIG3_CTXTC   0x00000200
#define CONFIG3_ITL     0x00000100
#define CONFIG3_LPA     0x00000080
#define CONFIG3_VEIC    0x00000040
#define CONFIG3_VINT    0x00000020
#define CONFIG3_SP      0x00000010
#define CONFIG3_CDMM    0x00000008
#define CONFIG3_MT      0x00000004
#define CONFIG3_SM      0x00000002
#define CONFIG3_TL      0x00000001

// Forward reference to CPU
class CPU;

class Coprocessor0 {
    private:
        // Coprocessor Register File
        COP0Register* registerFile[32][32];
    
        // Atomic bool for controlling count/compare loop
        std::atomic<bool> countCompActive;
    
        // Count Compare Thread
        std::thread* countCompThread;
    
        // Private thread count/compare loop
        void countCompare(CPU* cpu);
    
    public:
        Coprocessor0();
        ~Coprocessor0();
    
        // Tests for various operating modes
        inline bool inKernelMode();
        inline bool inSupervisorMode();
        inline bool inUserMode();
        bool interruptsEnabled();

        // Returns the value in a register
        uint32_t getRegister(uint8_t regnum, uint8_t sel);
    
    #ifdef TEST_PROJECT
        // Gets the reset value for a register, for unit testing
        uint32_t getRegisterReset(uint8_t regnum, uint8_t sel);
    #endif
    
        // Sets a register in software mode (i.e. mips programs) atomically
        void setRegisterSW(uint8_t regnum, uint8_t sel, uint32_t value);
    
        // Sets a register in hardware mode (i.e. mmu) atomically
        void setRegisterHW(uint8_t regnum, uint8_t sel, uint32_t value);
    
        // Does an atomic and on the register with mask
        void andRegisterHW(uint8_t regnum, uint8_t sel, uint32_t mask);
        
        // Does an atomic or on the register with mask
        void orRegisterHW(uint8_t regnum, uint8_t sel, uint32_t mask);
    
        // Resets a register to its original startup state atomically
        void resetRegister(uint8_t regnum, uint8_t sel);
    
        // Called by CPU to start and stop the count/compare registers
        void startCounter(CPU* cpu);
        void stopCounter();
};

#endif /* Coprocessor0_h */
