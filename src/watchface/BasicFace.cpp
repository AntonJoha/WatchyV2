#include "BasicFace.h"
#include "../Watchy.h"


void BasicFace::draw(void * data){
    auto display = Watchy::getDisplay();
    tmElements_t currentTime = Watchy::getTime();

    display->setFont(&DSEG7_Classic_Bold_53);

    display->setCursor(5, 53+60);
    if(currentTime.Hour < 10){
        display->print("0");
    }
    display->print(currentTime.Hour);
    display->print(":");
    if(currentTime.Minute < 10){
        display->print("0");
    }  
    display->println(currentTime.Minute);

}

