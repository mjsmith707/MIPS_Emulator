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
            std::string message;
        public:
            STest_Assertion(const char* msg) {
                message = msg;
            }
            STest_Assertion(std::string& msg) {
                message = msg;
            }
        
            virtual const char* what() const throw() {
                return message.c_str();
            }
    };
    
    // Runs testing routine on all registered tests
    void run_tests();
    
    // Adds a function to the list of tests
    void add_test(const char* category, const char* name, void (*func)());
    
    // Assertion templates
    template <typename T> void ASSERT_EQUAL(T val1, T val2) {
        if (val1 != val2) {
            std::string msg = "ASSERT_EQUAL FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    template <typename T> void ASSERT_NOT_EQUAL(T val1, T val2) {
        if (val1 == val2) {
            std::string msg = "ASSERT_NOT_EQUAL FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    template <typename T> void ASSERT_LESS_THAN(T val1, T val2) {
        if (val1 >= val2) {
            std::string msg = "ASSERT_LESS_THAN FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    template <typename T> void ASSERT_LESS_THAN_EQUAL(T val1, T val2) {
        if (val1 > val2) {
            std::string msg = "ASSERT_LESS_THAN_EQUAL FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    template <typename T> void ASSERT_GREATER_THAN(T val1, T val2) {
        if (val1 <= val2) {
            std::string msg = "ASSERT_GREATER_THAN FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    template <typename T> void ASSERT_GREATER_THAN_EQUAL(T val1, T val2) {
        if (val1 < val2) {
            std::string msg = "ASSERT_GREATER_THAN_EQUAL FAILED:\n\t\t\tExpected: ";
            msg += std::to_string(val1);
            msg += " Got: ";
            msg += std::to_string(val2);
            throw STest_Assertion(msg);
        }
    }
    
    void TEST_NOT_IMPLEMENTED();
}

#endif /* STest_h */