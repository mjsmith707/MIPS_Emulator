//
//  FileLoader.h
//  MIPS_Emulator
//
//  Created by Matt on 5/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef FileLoader_h
#define FileLoader_h

#include "ConsoleUI.h"
#include "CPU.h"
#include "PMMU.h"
#include "elfio/elfio.hpp"
#include <vector>
#include <string>
#include <algorithm>

class FileLoader {
    private:
        // Argument Type Enumeration, Value is priority
        enum ArgTypeEnum {
            ELF = 0,
            KERNEL = 1,
            INITRD = 2,
            RAW = 3,
            INVALID = 100,
        };
    
        // Argument type mapping for parsing
        typedef struct ArgType {
            ArgTypeEnum type;
            std::string name;
            uint32_t argc;
        } ArgType;
    
        // List of valid arguments
        // Enum type, matching string, number of parameters expected
        const std::vector<ArgType> ValidArguments = {
            {ArgTypeEnum::KERNEL, "-kernel", 1},
            {ArgTypeEnum::ELF, "-elf", 1},
            {ArgTypeEnum::INITRD, "-initrd", 1},
            {ArgTypeEnum::RAW, "-raw", 1},
        };
    
        // Parsed argument containing type and path
        typedef struct ParsedArg {
            ArgTypeEnum type;
            std::vector<std::string> args;
        } ParsedArg;
    
        // List of parsed command line arguments
        std::vector<ParsedArg> parsedArguments;
    
        // Argument sort comparator
        static bool parsedArgComp(const ParsedArg& a, const ParsedArg& b);
    
        // Executes a command line argument
        void executeArgument(ParsedArg& arg, ConsoleUI* consoleUI, CPU* cpu, PMMU* memory);
    
        // Loads a raw file to an address
        void loadRaw(uint32_t addr, ConsoleUI* consoleUI, std::string filename, PMMU* memory);
    
        // Loads an elf file and returns the entry point
        uint32_t loadElfFile(ConsoleUI* consoleUI, std::string filename, PMMU* memory);
    
    public:
        // Help Banner
        const std::string usageInfo = "Usage: ./mips.x <elf filename> OR\n \
        Other parameters:\n \
        '-elf <elf binary>'     Load ELF File and set PC to entry point\n \
        '-kernel <kernel file>' Load Kernel to 0x81000000 (for u-boot) \n \
        '-initrd <initrd file>' Load initrd.gz to 0x82000000 (for u-boot)\n \
        '-raw <flat binary>'    Load flat binary and set PC to 0xbfc00000";
    
        FileLoader();
    
        // Parses command line arguments and executes them according to priority
        void parseArguments(int argc, const char * argv[], ConsoleUI* consoleUI, CPU* cpu, PMMU* memory);
};

#endif /* FileLoader_h */
