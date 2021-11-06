#include "AppFrame.h"
#include "../Watchy.h"


AppFrame::AppFrame() {};

int AppFrame::handleButtonPress(uint64_t wakeupBit, void * data)
{
    if (wakeupBit & BACK_BTN_MASK) return MAIN_MENU_STATE;
    else return APP_STATE;
}

void AppFrame::draw(void * data){

    auto display = Watchy::getDisplay();
    auto currentTime = Watchy::getTime();
    
    display->init(0, false); //_initial_refresh to false to prevent full update on init
    display->setFullWindow();
    
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

    display->display(false); //partial refresh
    display->hibernate();
}

int AppFrame::run() {return APP_STATE;}

bool AppFrame::runnable() {return false;}
