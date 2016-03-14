//
//  COP0Register.h
//  MIPS_Emulator
//
//  Created by Matt on 1/8/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef COP0Register_h
#define COP0Register_h

// This is basically a C&P job from the C# version until I can think of a better way to implement it

#include <cstdint>  // uint32_t
#include <thread>
#include <mutex>
#include <stdexcept>

class COP0Register {
    private:
        enum REGBitRW {
            LOCKED = 0,
            READ = 1,
            READWRITE = 2
        };
    
        // Register bit controls
        REGBitRW bitfields[32];
    
        // Sets the bitfields mask
        void setRWMask(uint32_t mask1, uint32_t mask2);
    
        // Initial reset value for the register
        const uint32_t resetValue;
    
        // The actual register
        // Publically available for performance reasons
        // Software should use setValue/getValue
        volatile uint32_t copregister;
    
        // Mutex
        std::mutex mute;
    
        // Updates the register based on rwx locking bits
        void updateValue(uint32_t value, bool hwmode);
    
    public:
        COP0Register();
        COP0Register(uint32_t value, uint32_t mask1, uint32_t mask2);
    
        // Public accessor
        uint32_t getValue();

#ifdef TEST_PROJECT
        // For unit testing
        uint32_t getResetValue();
#endif
        // Public setter
        void setValue(uint32_t value, bool hwmode);
    
        // Does an atomic and with the register and value
        void andValue(uint32_t value, bool hwmode);
    
        // Does an atomic or with the register and value
        void orValue(uint32_t value, bool hwmode);
    
        // Does an atomic add
        void addValue(uint32_t value, bool hwmode);
    
        // Public resetter
        void resetRegister();
};

#endif /* COP0Register_h */
