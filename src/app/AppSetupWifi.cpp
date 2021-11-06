#include "AppSetupWifi.h"
#include "../Watchy.h"

void SetupWifi::draw(void * data)
{
    return;
}

bool SetupWifi::runnable() {return true;}

int SetupWifi::handleButtonPress(uint64_t wakeupBit, void * data)
{
    return MAIN_MENU_STATE;
}

int SetupWifi::run()
{
    WiFiManager wifiManager;
    auto display = Watchy::getDisplay();
    wifiManager.resetSettings();
    wifiManager.setTimeout(WIFI_AP_TIMEOUT);
    wifiManager.setAPCallback(Watchy::_configModeCallback);
    if(!wifiManager.autoConnect(WIFI_AP_SSID)) {//WiFi setup failed
        display->init(0, false); //_initial_refresh to false to prevent full update on init
        display->setFullWindow();
        display->fillScreen(GxEPD_BLACK);
        display->setFont(&FreeMonoBold9pt7b);
        display->setTextColor(GxEPD_WHITE);
        display->setCursor(0, 30);
        display->println("Setup failed &");
        display->println("timed out!");
        display->display(false); //full refresh
        display->hibernate();
    }else{
        display->init(0, false);//_initial_refresh to false to prevent full update on init
        display->setFullWindow();
        display->fillScreen(GxEPD_BLACK);
        display->setFont(&FreeMonoBold9pt7b);
        display->setTextColor(GxEPD_WHITE);
        display->println("Connected to");
        display->println(WiFi.SSID());
        display->display(false);//full refresh
        display->hibernate();
    }
  //turn off radios
    WiFi.mode(WIFI_OFF);
    btStop();

    return APP_STATE;

}