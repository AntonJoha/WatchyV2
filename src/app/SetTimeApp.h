#ifndef SET_TIME_APP_H
#define SET_TIME_APP_H
#include "../Watchy.h"

class SetTime : AppFrame{

    public:
        SetTime();
        bool runnable() {return true;};
        int run();
        void draw(void* data);
        int handleButtonPress(uint64_t wakeupBit, void* data);
};

#endif