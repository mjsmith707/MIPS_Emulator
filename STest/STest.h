//
//  STest.h
//  MIPS_Emulator
//
//  Created by Matt on 1/28/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef STest_h
#define STest_h

#include <iostream>
#include <iomanip>
#include <sstream> // Pretty unfortunately really...
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

// Simple Testing Framework

namespace STest {
    typedef std::tuple<std::string, void(*)()> testFunction_t;
    typedef std::tuple<std::string, std::vector<testFunction_t>> testCategory_t;
    
    // List of all registered tests
    extern std::vector<testCategory_t> STest_Functions;
    
    // Test exception class
    class STest_Assertion : public std::exception {
        private:
            const std::string message;
            const std::string expected;
            const std::string got;
        
        public:
            STest_Assertion(const char* msg, const char* expected, const char* got)
            : message(msg), expected(expected), got(got) {
            }
        
            STest_Assertion(std::string& msg, std::string& expected, std::string& got)
            : message(msg), expected(expected), got(got) {
            }
        
            STest_Assertion(std::string& msg)
            : message(msg), expected(""), got("") {
            }
        
            STest_Assertion(const char* msg)
            : message(msg), expected(""), got("") {
            }
        
            virtual const char* what() const throw() {
                return message.c_str();
            }
        
            std::string expectedValue() const {
                return expected;
            }
            
            std::string gotValue() const {
                return got;
            }
    };
    
    // Runs testing routine on all registered tests
    void run_tests();
    
    // Adds a function to the list of tests
    void add_test(const char* category, const char* name, void (*func)());
    
    // Assertion templates
    template <typename T> void ASSERT_EQUAL(T val1, T val2) {
        if (val1 != val2) {
            std::string msg = "ASSERT_EQUAL FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    template <typename T> void ASSERT_NOT_EQUAL(T val1, T val2) {
        if (val1 == val2) {
            std::string msg = "ASSERT_NOT_EQUAL FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    template <typename T> void ASSERT_LESS_THAN(T val1, T val2) {
        if (val1 >= val2) {
            std::string msg = "ASSERT_LESS_THAN FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    template <typename T> void ASSERT_LESS_THAN_EQUAL(T val1, T val2) {
        if (val1 > val2) {
            std::string msg = "ASSERT_LESS_THAN_EQUAL FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    template <typename T> void ASSERT_GREATER_THAN(T val1, T val2) {
        if (val1 <= val2) {
            std::string msg = "ASSERT_GREATER_THAN FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    template <typename T> void ASSERT_GREATER_THAN_EQUAL(T val1, T val2) {
        if (val1 < val2) {
            std::string msg = "ASSERT_GREATER_THAN_EQUAL FAILED:";
            std::stringstream ss1;
            std::stringstream ss2;
            ss1 << "0x" << std::hex << val1;
            ss2 << "0x" << std::hex << val2;
            std::string expected = ss1.str();
            std::string got = ss2.str();
            throw STest_Assertion(msg, expected, got);
        }
    }
    
    void TEST_NOT_IMPLEMENTED();
}

#endif /* STest_h */