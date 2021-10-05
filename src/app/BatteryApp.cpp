
#include "BatteryApp.h"
#include "../Watchy.h"


void BatteryApp::draw(void *data)
{
    auto display = Watchy::getDisplay();

    display.init(0, false); //_initial_refresh to false to prevent full update on init
    display.setFullWindow();
    display.fillScreen(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(20, 30);
    display.println("Battery Voltage:");
    float voltage = Watchy::getBatteryVoltage();
    display.setCursor(70, 80);
    display.print(voltage);
    display.println("VB");
    display.display(false); //full refresh
    display.hibernate();

}
        
int BatteryApp::handleButtonPress(uint64_t wakeupBit, void* data)
{
    if (wakeupBit & BACK_BTN_MASK) return MAIN_MENU_STATE;
    else return APP_STATE;
}

BatteryApp::BatteryApp() {}

int BatteryApp::run() {
    draw(nullptr);
    return MAIN_MENU_STATE;
}

bool BatteryApp::runnable() {return false;}