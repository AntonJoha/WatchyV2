#ifndef APP_FRAME_H
#define APP_FRAME_H
#include <Arduino.h>

#include "../config.h"

class AppFrame{

    public:
        virtual void draw() = 0;
        virtual int handleButtonPress(uint64_t wakeupBit) = 0;
        AppFrame();
        virtual bool runnable() = 0;
        virtual int run() = 0;
};

#endif
