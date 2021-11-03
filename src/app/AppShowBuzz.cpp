#include "AppShowBuzz.h"
#include "../Watchy.h"


void ShowBuzz::draw(void * data)
{
    return;
}

int ShowBuzz::handleButtonPress(uint64_t wakeupBit, void* data)
{
    return MAIN_MENU_STATE;
} 

int ShowBuzz::run(){
    auto display = Watchy::getDisplay();
    display.init(0, false); //_initial_refresh to false to prevent full update on init
    display.setFullWindow();
    display.fillScreen(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(70, 80);
    display.println("Buzz!");
    display.display(false); //full refresh
    display.hibernate();
    Watchy::vibMotor();
    return MAIN_MENU_STATE;
}

bool ShowBuzz::runnable() {return true;}