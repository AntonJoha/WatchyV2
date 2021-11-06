#include "FaceFrame.h"



int FaceFrame::handleButtonPress(uint64_t wakeUpBit, void * watchData){
    if (wakeUpBit & MENU_BTN_MASK) return MAIN_MENU_STATE;
    return WATCHFACE_STATE;
}