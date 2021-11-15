#ifndef APP_SETUP_WIFI_H
#define APP_SETUP_WIFI_H
#include "AppFrame.h"

class SetupWifi : public AppFrame{

    public:
        int handleButtonPress(uint64_t wakeupBit);
        void draw();
        bool runnable();
        int run();
};


#endif
