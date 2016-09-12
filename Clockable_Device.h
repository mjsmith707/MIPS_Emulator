//
//  Clockable_Device.h
//  MIPS_Emulator
//
//  Created by Matt on 7/10/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef Clockable_Device_h
#define Clockable_Device_h

#include "SharedDefs.h"

// Interface for devices which rely on strict CPU clock timing

class Clockable_Device {
    public:
        // Used to get the interval between executions
        virtual uint32_t getCycleInterval() = 0;
    
        // Indicates to the CPU that the interval should be disabled
        virtual bool isEnabled() = 0;
    
        // Called by CPU when cycle interval is reached
        virtual void onCycleExecute() = 0;
};

#endif /* Clockable_Device_h */
