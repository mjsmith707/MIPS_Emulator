//
//  main.cpp
//  DispatchTests
//
//  Created by Matt on 4/19/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <atomic>
using namespace std::chrono;

// Shared vars
int a;
const int b = 1;
const int dividend = 2;
const int bound = 100;
std::atomic<bool> haltVar;
unsigned long cycleCounter;

#ifdef GOTO
// Computed Goto
int compGoto();
#endif

#ifdef SWITCH
// Switch dispatch
int switchDispatch();
#endif

#ifdef FUNC
// Function Table dispatch
int functionTable();
void add(int*);
void branch(int*);
void sub(int*);
void divide(int*);
void halt(int*);
#endif

#ifdef POLY
// Object Polymorphism
int polymorphicTable();
class Instruction {
    public:
        virtual void execute(int*) = 0;
};

class AddInst : public Instruction {
    public:
    void execute(int* pc) {
        a += b;
        (*pc)++;
    }
};

class BranchInst : public Instruction {
public:
    void execute(int* pc) {
        if (a < bound) {
            (*pc) = 0;
        }
        else {
            (*pc)++;
        }
    }
};

class SubInst : public Instruction {
public:
    void execute(int* pc) {
        a -= b;
        (*pc)++;
    }
};

class DivInst : public Instruction {
public:
    void execute(int* pc) {
        a /= dividend;
        (*pc)++;
    }
};

class HaltInst : public Instruction {
public:
    void execute(int* pc) {
        (*pc)++;
    }
};
#endif

int main(int argc, const char * argv[]) {
    int secondsArray[] = {1,5,10,30,60};
    std::cout << std::fixed << std::setprecision(8);

#ifdef GOTO
    // Goto
    for (int i=0; i<5; i++) {
        int seconds = secondsArray[i];
        
        std::thread thread1(compGoto);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        haltVar = true;
        thread1.join();
        unsigned long gotoCycleCount = cycleCounter;
        std::cout << seconds << ", " << gotoCycleCount << std::endl;
    }
#endif
    
#ifdef SWITCH
    // Switch
    for (int i=0; i<5; i++) {
        int seconds = secondsArray[i];
        
        std::thread thread2(switchDispatch);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        haltVar = true;
        thread2.join();
        unsigned long switchCycleCount = cycleCounter;
        
        std::cout << seconds << ", " << switchCycleCount <<std::endl;
    }
#endif
    
#ifdef FUNC
    // Function Table
    for (int i=0; i<5; i++) {
        int seconds = secondsArray[i];
        
        std::thread thread3(functionTable);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        haltVar = true;
        thread3.join();
        unsigned long functionCycleCount = cycleCounter;
        
        std::cout << seconds << ", " << functionCycleCount <<std::endl;
    }
#endif
    
#ifdef POLY
    // Polymorphic Dynamic Dispatch
    for (int i=0; i<5; i++) {
        int seconds = secondsArray[i];
        
        std::thread thread4(polymorphicTable);
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        haltVar = true;
        thread4.join();
        unsigned long polyCycleCount = cycleCounter;
        
        std::cout << seconds << ", " << polyCycleCount <<std::endl;
    }
#endif
    
    return 0;
}

#ifdef GOTO
// 0   1   2   3   4
// Add bl sub div halt
// Computed goto
int compGoto() {
    cycleCounter = 0;
    haltVar = false;
    const void* program[5] = {&&Add, &&Bl, &&Div, &&Sub, &&Halt};
    
#define DISPATCH() cycleCounter++; if (haltVar == true) return a; goto *program[pc++]
start:
    int pc = 0;
    a = 0;
    
    DISPATCH();
    
    Add:
        a += b;
        DISPATCH();
    
    Bl:
        if (a < bound) {
            pc = 0;
        }
        DISPATCH();
    
    Sub:
        a -= b;
        DISPATCH();
    
    Div:
        a /= dividend;
        DISPATCH();
    
    Halt:
        goto start;
}
#endif

#ifdef SWITCH
// Switch based dispatch
int switchDispatch() {
    cycleCounter = 0;
    haltVar = false;
    const int program[5] = {0,1,3,2,4};
    
start:
    int pc = 0;
    a = 0;
    
    for (;;) {
        if (haltVar == true) {
            return a;
        }
        cycleCounter++;
        switch (program[pc++]) {
        case 0:
            a += b;
            break;
                
        case 1:
            if (a < bound) {
                pc = 0;
            }
            else {
                pc++;
            }
            break;
                
        case 2:
            a -= b;
            pc++;
            break;
        
        case 3:
            a /= dividend;
            break;
        
        case 4:
        default:
            goto start;
        }
    }
}
#endif

#ifdef FUNC
// Function table dispatch
int functionTable() {
    haltVar = false;
    cycleCounter = 0;
    void (*program[5])(int*) = {add,branch,divide,sub,halt};
    
start:
    a = 0;
    int pc = 0;
    
    while (pc < 5) {
        if (haltVar == true) {
            return a;
        }
        else {
            program[pc](&pc);
            cycleCounter++;
        }
    }
    goto start;
}

void add(int* pc) {
    a += b;
    (*pc)++;
}

void branch(int* pc) {
    if (a < bound) {
        *pc = 0;
    }
    else {
        (*pc)++;
    }
}

void sub(int* pc) {
    a -= b;
    (*pc)++;
}

void divide(int* pc) {
    a /= dividend;
    (*pc)++;
}

void halt(int* pc) {
    (*pc)++;
}
#endif

#ifdef POLY
int polymorphicTable() {
    haltVar = false;
    cycleCounter = 0;
    Instruction* program[5];
    program[0] = new AddInst();
    program[1] = new BranchInst();
    program[2] = new DivInst();
    program[3] = new SubInst();
    program[4] = new HaltInst();
    
start:
    a = 0;
    int pc = 0;
    
    while (pc < 5) {
        if (haltVar == true) {
            return a;
        }
        else {
            program[pc]->execute(&pc);
            cycleCounter++;
        }
    }
    goto start;
}
#endif
