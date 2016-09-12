//
//  Interruptable_Device.h
//  MIPS_Emulator
//
//  Created by Matt on 7/10/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef Interruptable_Device_h
#define Interruptable_Device_h

#include "CPU.h"

class Interruptable_Device {
public:
    // Gets a pointer to the CPU so it can call sendInterrupt etc.
    // More longterm this needs to point to an interrupt controller class
    virtual void getCPUHandle(CPU* cpu) = 0;
};

#endif /* Interruptable_Device_h */
