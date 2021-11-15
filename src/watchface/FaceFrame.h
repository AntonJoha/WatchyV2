#ifndef FACEFRAME_H
#define FACEFRAME_H
#include <Arduino.h>
#include "../config.h"

class FaceFrame{

    public:
        virtual void draw() = 0;
        int handleButtonPress(uint64_t wakeUpBit);
};

#endif
