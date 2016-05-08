//
//  FileLoader.cpp
//  MIPS_Emulator
//
//  Created by Matt on 5/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "FileLoader.h"

FileLoader::FileLoader() {
    
}

// Basic formats for now.. I've got big plans with all this..
// -kernel <elf>
// -loader <elf>
// -initrd <binary>
// <elf>
// Takes command line arguments and performs work
void FileLoader::parseArguments(int argc, const char * argv[], ConsoleUI* consoleUI, CPU* cpu, PMMU* memory) {
    if (consoleUI == nullptr) {
        throw std::runtime_error("Got consoleUI nullptr in FileLoader::parseArguments()");
    }
    if (cpu == nullptr) {
        throw std::runtime_error("Got cpu nullptr in FileLoader::parseArguments()");
    }
    if (memory == nullptr) {
        throw std::runtime_error("Got memory nullptr in FileLoader::parseArguments()");
    }
    if (argc < 2) {
        std::cerr << usageInfo << std::endl;
        exit(0);
    }
    
    parsedArguments.clear();
    
    // First pass, find arguments if any
    bool foundArg = false;
    for (int i=1; i<argc; i++) {
        for (auto& arg : ValidArguments) {
            if (arg.name.compare(argv[i]) == 0) {
                foundArg = true;
                break;
            }
        }
        if (foundArg) {
            break;
        }
    }
    
    // Assume single file with no parameters
    // E.g. ./mips.x <elf filename>
    if (!foundArg) {
        uint32_t entrypoint = loadElfFile(consoleUI, argv[1], memory);
        cpu->setPC(entrypoint);
        return;
    }
    else {
        // Parse arguments
        int i=1;
        // Look for argument
        while (i < argc) {
            foundArg = false;
            ArgType argtype;
            for (auto& arg : ValidArguments) {
                if (arg.name.compare(argv[i]) == 0) {
                    foundArg = true;
                    argtype = arg;
                    i++;
                    break;
                }
            }
            
            // If found argument name then find parameters
            if (foundArg) {
                uint32_t paramCount = 0;
                std::vector<std::string> params;
                // Consume parameters
                while ((i < argc) && (paramCount < argtype.argc)) {
                    // Add parameter
                    params.push_back(argv[i]);
                    paramCount++;
                    i++;
                }
                
                // Check if we read the proper amount of parameters
                if (paramCount != argtype.argc) {
                    std::cerr << "Incorrect number of parameters for " << argtype.name << " expected " << argtype.argc << " but got " << paramCount << std::endl;
                    exit(-1);
                }
                
                // Make parsed argument
                ParsedArg parg = {argtype.type, params};
                parsedArguments.push_back(parg);
            }
            else {
                std::cerr << "Invalid argument specified: " << argv[i] << std::endl;
                std::cerr << usageInfo << std::endl;
                exit(-1);
            }
        }
    }
    
    // If we didn't parse any arguments then error
    if (parsedArguments.size() == 0) {
        std::cerr << usageInfo << std::endl;
        exit(0);
    }
    
    // Sort arguments by priority, least to highest
    std::make_heap(parsedArguments.begin(), parsedArguments.end(), parsedArgComp);
    
    // Pop and execute
    while (!parsedArguments.empty()) {
        ParsedArg parg = parsedArguments.front();
        std::pop_heap(parsedArguments.begin(), parsedArguments.end(), parsedArgComp);
        parsedArguments.pop_back();
        
        executeArgument(parg, consoleUI, cpu, memory);
    }
}

// Argument sort comparator
bool FileLoader::parsedArgComp(const ParsedArg& a, const ParsedArg& b) {
    return static_cast<int>(a.type) < static_cast<int>(b.type);
}

// Executes a command line argument
// Motivation for this was storing kernel and initrd in memory for u-boot to use
// http://processors.wiki.ti.com/index.php/Booting_Linux_kernel_using_U-Boot#SDRAM
void FileLoader::executeArgument(ParsedArg& arg, ConsoleUI* consoleUI, CPU* cpu, PMMU* memory) {
    switch (arg.type) {
        // Elf Binary
        case ELF: {
            uint32_t entrypoint = loadElfFile(consoleUI, arg.args.at(0), memory);
            cpu->setPC(entrypoint);
            break;
        }
        // Kernel (aka vmlinux)
        case KERNEL: {
            uint32_t loadaddr = 0x81000000;
            loadRaw(loadaddr, consoleUI, arg.args.at(0), memory);
            break;
        }
        // Initrd.gz (aka for Linux)
        case INITRD: {
            uint32_t loadaddr = 0x82000000;
            loadRaw(loadaddr, consoleUI, arg.args.at(0), memory);
            break;
        }
        // Load Raw file
        case RAW: {
            uint32_t loadaddr = 0xbfc00000;
            loadRaw(loadaddr, consoleUI, arg.args.at(0), memory);
            cpu->setPC(loadaddr);
            break;
        }
        case INVALID:
        default: {
            throw std::runtime_error("Got invalid argument type in FileLoader::executeArgument");
        }
    }
}

// Loads a raw file to an address
void FileLoader::loadRaw(uint32_t addr, ConsoleUI* consoleUI, std::string filename, PMMU* memory) {
    // Open file
    std::fstream fhandle;
    fhandle.open(filename, std::ios::in | std::ios::binary);
    if (!fhandle.is_open()) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        exit(-1);
    }
    
    // Get file length
    fhandle.seekg(0, fhandle.end);
    size_t len = fhandle.tellg();
    fhandle.seekg(0, fhandle.beg);
    
    // Malloc buffer
    char* buff = new char[len];
    
    // Read file
    fhandle.read(buff, len);
    if (!fhandle) {
        std::cerr << "Failed to read file: " << filename << ", only read " << fhandle.gcount() << " bytes" << std::endl;
        exit(-1);
    }
    
    // Write to memory
    for (size_t i=0; i<len; i++, addr++) {
        memory->storeBytePhys(addr, buff[i]);
    }
    
    // Cleanup
    delete[] buff;
}

// Loads an elf file and returns the entry point
uint32_t FileLoader::loadElfFile(ConsoleUI* consoleUI, std::string filename, PMMU* memory) {
    // Load file
    ELFIO::elfio reader;
    if (!reader.load(filename)) {
        std::cerr << "Failed to load file: " << filename << std::endl;
        exit(-1);
    }
    
    // Print ELF file properties
    std::stringstream ss;
    ss << "ELF file class    : ";
    if (reader.get_class() == ELFCLASS32) {
        ss << "ELF32" << std::endl;
    }
    else {
        ss << "ELF64" << std::endl;
        std::cerr << "64bit Elf file \" " << filename << "\" is not supported." << std::endl;
        exit(-1);
    }
    
    ss << "ELF file encoding : ";
    if (reader.get_encoding() == ELFDATA2LSB) {
        ss << "Little endian" << std::endl;
        std::cerr << "Little endian\" " << filename << "\" is not supported." << std::endl;
        exit(-1);
    }
    else {
        ss << "Big endian" << std::endl;
    }
    
    ss << std::hex << "ELF Entry Point: 0x" << reader.get_entry() << std::endl;
    
    // Print ELF file sections info
    ELFIO::Elf_Half sec_num = reader.sections.size();
    ss << "Number of sections: " << sec_num << std::endl;
    for ( int i = 0; i < sec_num; ++i ) {
        ELFIO::section* psec = reader.sections[i];
        ss << "  [" << i << "] "
        << psec->get_name()
        << "\t"
        << psec->get_size()
        << std::endl;
        // Access to section's data
        // const char* p = reader.sections[i]->get_data()
    }
    
    // Print ELF file segments info
    ELFIO::Elf_Half seg_num = reader.segments.size();
    ss << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
        const ELFIO::segment* pseg = reader.segments[i];
        ss << "  [" << i << "] 0x" << std::hex
        << pseg->get_flags()
        << "\t0x"
        << pseg->get_virtual_address()
        << "\t0x"
        << pseg->get_file_size()
        << "\t0x"
        << pseg->get_memory_size()
        << std::endl;
        
        // Access to segments's data
        const char* p = pseg->get_data();
        if (p != NULL) {
            // Load segment into memory
            uint32_t addr = (uint32_t)pseg->get_virtual_address();
            for (uint32_t j=0; j < pseg->get_file_size(); j++, addr++) {
                memory->storeBytePhys(addr, p[j]);
            }
        }
    }
    consoleUI->sendConsoleMsg(ss.str());
    return (uint32_t)reader.get_entry();
}

/*
 // Very much a work in progress. I'm still not sure how Linux actually expects these arguments to be.
 void setBootArgs(ConsoleUI* consoleUI, const char* str, PMMU* memory, CPU* cpu) {
 // Store in some low area of memory
 uint32_t baseaddr = 0xA0001000;
 uint32_t addr = baseaddr;
 uint32_t count = 1;
 while (*str) {
 memory->storeBytePhys(addr, *str);
 str++;
 addr++;
 if (*str == ' ') {
 count++;
 }
 }
 cpu->setRegister(4, count);
 cpu->setRegister(5, baseaddr);
 cpu->setRegister(6, 0);
 cpu->setRegister(7, 0);
 }
 */