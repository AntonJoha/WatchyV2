#ifndef APP_SETUP_WIFI_H
#define APP_SETUP_WIFI_H
#include "AppFrame.h"

class SetupWifi : public AppFrame{

    public:
        int handleButtonPress(uint64_t wakeupBit, void * data);
        void draw(void * data);
        bool runnable();
        int run();
};


#endif