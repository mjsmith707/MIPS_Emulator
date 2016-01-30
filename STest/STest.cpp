//
//  STest.cpp
//  MIPS_Emulator
//
//  Created by Matt on 1/28/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "STest.h"

std::vector<STest::testCategory_t> STest::STest_Functions;

// Runs all registered test functions
void STest::run_tests() {
    size_t passCount = 0;
    size_t totalCount = 0;
    
    for (auto& category : STest_Functions) {
        std::cout << std::left << "== \x1b[33m" << std::get<0>(category) << "\x1b[39;49m ==" << std::endl;
        totalCount += std::get<1>(category).size();
        size_t localPassCount = 0;
        size_t localTotalCount = 0;
        localTotalCount = std::get<1>(category).size();
        for (auto& t : std::get<1>(category)) {
            size_t len = std::get<0>(t).size() + 3;
            try {
                std::cout << std::left << "- \x1b[33m" << std::get<0>(t) << "\x1b[39;49m ";
                std::get<1>(t)();
                std::cout << std::right << std::setw((int32_t)(80-len) > 0 ? (int32_t)(80-len) : 0) << "[\x1b[32mPASSED\x1b[39;49m]" << std::endl;
                passCount++;
                localPassCount++;
            }
            catch (STest::STest_Assertion& e) {
                std::cout << std::right << std::setw((int32_t)(80-len) > 0 ? (int32_t)(80-len) : 0) << "[\x1b[31mFAILED\x1b[39;49m]" << std::endl;
                std::cout << std::left << "\t" << e.what() << std::endl;
            }
        }
        
        std::cout << std::endl << std::left << localPassCount << "/" << localTotalCount << " Tests Completed Successfully." << std::endl << std::endl;
    }
    std::cout << std::left << "Total: " << passCount << "/" << totalCount << " Tests Completed Successfully." << std::endl;
}

// Adds a function to the list of tests
void STest::add_test(const char* category, const char* name, void (*func)()) {
    std::string catstr = category;
    for (auto& category : STest_Functions) {
        if (std::get<0>(category).compare(catstr) == 0) {
            std::get<1>(category).push_back(std::make_tuple(name, func));
            return;
        }
    }
    std::vector<testFunction_t> catfuncs;
    catfuncs.push_back(std::make_tuple(name, func));
    STest_Functions.push_back(std::make_tuple(catstr, catfuncs));
}

// Assertion for unfinished test functions
void STest::TEST_NOT_IMPLEMENTED() {
    throw STest_Assertion("TEST NOT IMPLEMENTED");
}