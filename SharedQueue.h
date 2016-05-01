//
//  SharedQueue.h
//  MIPS_Emulator
//
//  Created by Matt on 2/5/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef SharedQueue_h
#define SharedQueue_h

#include <queue>
#include <mutex>

template <typename T> class Shared_Queue {
    private:
        std::queue<T> sharedQueue;
        std::mutex queueMutex;
        
    public:
        Shared_Queue() {
            
        }
        
        T pop() {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!sharedQueue.empty()) {
                T result = sharedQueue.front();
                sharedQueue.pop();
                return result;
            }
            else {
                throw std::runtime_error("Shared Queue is Empty");
            }
        }
        
        void push(T input) {
            std::lock_guard<std::mutex> lock(queueMutex);
            sharedQueue.push(input);
        }
        
        bool isEmpty() {
            std::lock_guard<std::mutex> lock(queueMutex);
            return sharedQueue.empty();
        }
};


#endif /* SharedQueue_h */
