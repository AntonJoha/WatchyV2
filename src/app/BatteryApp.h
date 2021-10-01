#ifndef BATTERY_APP_H
#define BATTERY_APP_H
#include "AppFrame.h"

class BatteryApp : public AppFrame{
    public:
        void draw(void *data);
        int handleButtonPress(uint64_t wakeupBit, void* data);
        BatteryApp();
        bool runnable();
        int run();

};


#endif