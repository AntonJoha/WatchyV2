#ifndef BATTERY_APP_H
#define BATTERY_APP_H
#include "AppFrame.h"

class BatteryApp : public AppFrame{
    public:
        void draw();
        int handleButtonPress(uint64_t wakeupBit);
        BatteryApp();
        bool runnable();
        int run();

};


#endif
