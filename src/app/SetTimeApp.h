#ifndef SET_TIME_APP_H
#define SET_TIME_APP_H
#include "../Watchy.h"

class SetTime : public AppFrame{

    public:
        SetTime();
        bool runnable() {return true;};
        int run();
        void draw();
        int handleButtonPress(uint64_t wakeupBit);
};

#endif
