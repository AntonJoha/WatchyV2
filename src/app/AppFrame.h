#ifndef APP_FRAME_H
#define APP_FRAME_H
#include <Arduino.h>
#define DATASIZE 2048
#include "../config.h"

class AppFrame{

    public:
        virtual void draw(void * data) = 0;
        virtual int handleButtonPress(uint64_t wakeupBit, void * data) = 0;
        AppFrame();
        virtual bool runnable() = 0;
        virtual int run() = 0;
};





#endif