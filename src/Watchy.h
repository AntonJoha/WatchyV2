#ifndef WATCHY_H
#define WATCHY_H

#include <Arduino.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <DS3232RTC.h>
#include <GxEPD2_BW.h>
#include <Wire.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "DSEG7_Classic_Bold_53.h"
#include "BLE.h"
#include "bma.h"
#include "config.h"    
#include "app/AppFrame.h"
#include "watchface/FaceFrame.h"

typedef struct weatherData{
    int8_t temperature;
    int16_t weatherConditionCode;
}weatherData;

namespace Watchy {
        extern DS3232RTC RTC;
        extern GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;
        extern tmElements_t currentTime;
        void init(String datetime = "");
        void deepSleep();
        tmElements_t getTime();
        float getBatteryVoltage();
        void vibMotor(uint8_t intervalMs = 100, uint8_t length = 20);
        GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> * getDisplay();
        DS3232RTC * getRTC();
        void handleButtonPress();
        void showMenu(byte menuIndex, bool partialRefresh);
        void showFastMenu(byte menuIndex);
        weatherData getWeatherData();
        void showWatchFace(bool partialRefresh);
        void drawWatchFace(); //override this method for different watch faces
        bool connectWiFi();
        void addApp(const char* name, AppFrame* (*factory)(void));
        
        void setFace(FaceFrame * frame);
        void _rtcConfig(String datetime);    
        void _bmaConfig();
        void _configModeCallback(WiFiManager *myWiFiManager);
        uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
}

extern FaceFrame *face;

extern RTC_DATA_ATTR int guiState;
extern RTC_DATA_ATTR BMA423 sensor;
extern RTC_DATA_ATTR bool WIFI_CONFIGURED;
extern RTC_DATA_ATTR bool BLE_CONFIGURED;
extern RTC_DATA_ATTR byte data[DATASIZE];
#endif