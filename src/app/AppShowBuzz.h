#ifndef APP_SHOW_BUZZ_H
#define APP_SHOW_BUZZ_H
#include "AppFrame.h"


class ShowBuzz: public AppFrame{
    public:
        void draw(void * data);
        int handleButtonPress(uint64_t wakeupBit, void * data);
        bool runnable();
        int run();
};


#endif