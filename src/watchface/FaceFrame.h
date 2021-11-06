#ifndef FACEFRAME_H
#define FACEFRAME_H
#include <Arduino.h>
#include "../config.h"

class FaceFrame{

    public:
        virtual void draw(void * data) = 0;
        int handleButtonPress(uint64_t wakeUpBit, void * data);
};

#endif