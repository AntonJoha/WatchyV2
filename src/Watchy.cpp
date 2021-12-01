#include "Watchy.h"
#include "app/AppMenu.h"
#include "app/BatteryApp.h"
#include "app/SetTimeApp.h"


DS3232RTC Watchy::RTC(false); 
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> Watchy::display(GxEPD2_154_D67(CS, DC, RESET, BUSY));
tmElements_t Watchy::currentTime;
AppMenu menu;
BatteryApp app;
FaceFrame* face;

RTC_DATA_ATTR int guiState;
RTC_DATA_ATTR int menuIndex;
RTC_DATA_ATTR BMA423 sensor;
RTC_DATA_ATTR bool WIFI_CONFIGURED;
RTC_DATA_ATTR bool BLE_CONFIGURED;
RTC_DATA_ATTR weatherData currentWeather;
RTC_DATA_ATTR int weatherIntervalCounter = WEATHER_UPDATE_INTERVAL;
RTC_DATA_ATTR byte data[DATASIZE];


void Watchy::fetchMemory(void * dest, size_t size){
	if (!allowedSize(size)) return;
	memcpy(dest, data, size);
}

void Watchy::uploadData(void * source, size_t size){
	if (!allowedSize(size)) return;
	memcpy(data, source, size);
}

bool Watchy::allowedSize(size_t size){
	return size == DATASIZE;
}


String getValue(String data, char seperator, int index){
    
    int pos = 0;
    int length = data.length();

    while (pos < length && index != 0)
    {
        if (data[pos] == seperator)
        {
            index--;
        }
        pos++;
    }
    if (pos >= length) return "";

    String toReturn = "";
    
    while(pos < length){
        if (data[pos] == seperator) break;
        toReturn += data[pos];
        ++pos;
    }
    return toReturn;
}

DS3232RTC* Watchy::getRTC()
{
    return &Watchy::RTC;
}

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>* Watchy::getDisplay()
{
    return &Watchy::display;
}

tmElements_t Watchy::getTime()
{
Watchy::RTC.read(currentTime);
    return currentTime;
}


void Watchy::addApp(const char * name, AppFrame* (*factory)(void))
{
    menu.addApp(name, factory);
}

void draw(){
    if(guiState == WATCHFACE_STATE){
        Watchy::showWatchFace(true); //partial updates on tick
    }
    else if (guiState == APP_STATE)
    {
        menu.appDraw();
    }
    else if (guiState == MAIN_MENU_STATE)
    {
        Watchy::showMenu(menuIndex, false);
    }
}

void Watchy::setFace(FaceFrame * value){
    face = value;
}

void Watchy::init(String datetime){
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause(); //get wake up reason
    Wire.begin(SDA, SCL); //init i2c

    switch (wakeup_reason)
    {
        #ifdef ESP_RTC
        case ESP_SLEEP_WAKEUP_TIMER: //ESP Internal RTC
            RTC.read(currentTime);
            currentTime.Minute++;
            tmElements_t tm;
            tm.Month = currentTime.Month;
            tm.Day = currentTime.Day;
            tm.Year = currentTime.Year;
            tm.Hour = currentTime.Hour;
            tm.Minute = currentTime.Minute;
            tm.Second = 0;
            time_t t = makeTime(tm);
            RTC.set(t);
            RTC.read(currentTime); 
            draw();
            break;        
        #endif
        case ESP_SLEEP_WAKEUP_EXT0: //RTC Alarm
            RTC.alarm(ALARM_2); //resets the alarm flag in the RTC
            if(guiState == WATCHFACE_STATE){
                RTC.read(currentTime);
                showWatchFace(true); //partial updates on tick
            }
            break;
        case ESP_SLEEP_WAKEUP_EXT1: //button Press
            handleButtonPress();
            break;
        default: //reset
            #ifndef ESP_RTC
            _rtcConfig(datetime);
            #endif
            _bmaConfig();
            showWatchFace(false); //full update on reset
            break;
    }
    deepSleep();
}

void Watchy::deepSleep(){
  #ifndef ESP_RTC
  esp_sleep_enable_ext0_wakeup(RTC_PIN, 0); //enable deep sleep wake on RTC interrupt
  #endif  
  #ifdef ESP_RTC
  esp_sleep_enable_timer_wakeup(60000000);
  #endif 
  esp_sleep_enable_ext1_wakeup(BTN_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); //enable deep sleep wake on button press
  esp_deep_sleep_start();
}

void Watchy::_rtcConfig(String datetime){
    if(datetime != NULL){
        const time_t FUDGE(30);//fudge factor to allow for upload time, etc. (seconds, YMMV)
        tmElements_t tm;
        tm.Year = getValue(datetime, ':', 0).toInt() - YEAR_OFFSET;//offset from 1970, since year is stored in uint8_t        
        tm.Month = getValue(datetime, ':', 1).toInt();
        tm.Day = getValue(datetime, ':', 2).toInt();
        tm.Hour = getValue(datetime, ':', 3).toInt();
        tm.Minute = getValue(datetime, ':', 4).toInt();
        tm.Second = getValue(datetime, ':', 5).toInt();

        time_t t = makeTime(tm) + FUDGE;
        RTC.set(t);

    }
    //https://github.com/JChristensen/DS3232RTC
    RTC.squareWave(SQWAVE_NONE); //disable square wave output
    //RTC.set(compileTime()); //set RTC time to compile time
    RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0); //alarm wakes up Watchy every minute
    RTC.alarmInterrupt(ALARM_2, true); //enable alarm interrupt
    RTC.read(currentTime);
}

void Watchy::handleButtonPress(){
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
  //Menu Button

    if (guiState == MAIN_MENU_STATE){
        guiState = menu.handleButtonPress(wakeupBit);
        if (guiState == WATCHFACE_STATE) {showWatchFace(false);}
    }
    else if (guiState == APP_STATE)
    {
        guiState = menu.apphandleButtonPress(wakeupBit);
    }
    else if(guiState == WATCHFACE_STATE){//enter menu state if coming from watch face
        guiState = face->handleButtonPress(wakeupBit);
        if (guiState == MAIN_MENU_STATE)
        {
            showMenu(menuIndex, false);
        }
    }
}

void Watchy::showMenu(byte menuIndex, bool partialRefresh){
    menu.draw();
    guiState = MAIN_MENU_STATE; 
}

void Watchy::vibMotor(uint8_t intervalMs, uint8_t length){
    pinMode(VIB_MOTOR_PIN, OUTPUT);
    bool motorOn = false;
    for(int i=0; i<length; i++){
        motorOn = !motorOn;
        digitalWrite(VIB_MOTOR_PIN, motorOn);
        delay(intervalMs);
    }
}

void Watchy::showWatchFace(bool partialRefresh){
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  drawWatchFace();
  display.display(partialRefresh); //partial refresh
  display.hibernate();
  guiState = WATCHFACE_STATE;
}

void Watchy::drawWatchFace(){
    face->draw();  
}

weatherData Watchy::getWeatherData(){
    if(weatherIntervalCounter >= WEATHER_UPDATE_INTERVAL){ //only update if WEATHER_UPDATE_INTERVAL has elapsed i.e. 30 minutes
        if(connectWiFi()){//Use Weather API for live data if WiFi is connected
            HTTPClient http;
            http.setConnectTimeout(3000);//3 second max timeout
            String weatherQueryURL = String(OPENWEATHERMAP_URL) + 
                String(CITY_NAME) + 
                String(",") + 
                String(COUNTRY_CODE) + 
                String("&units=") + 
                String(TEMP_UNIT) + 
                String("&appid=") + 
                String(OPENWEATHERMAP_APIKEY);

            http.begin(weatherQueryURL.c_str());
            int httpResponseCode = http.GET();
            if(httpResponseCode == 200) {
                String payload = http.getString();
                JSONVar responseObject = JSON.parse(payload);
                currentWeather.temperature = int(responseObject["main"]["temp"]);
                currentWeather.weatherConditionCode = int(responseObject["weather"][0]["id"]);            
            }else{
                //http error
            }
            http.end();
            //turn off radios
            WiFi.mode(WIFI_OFF);
            btStop();
        }else{//No WiFi, use RTC Temperature
            uint8_t temperature = RTC.temperature() / 4; //celsius
            if(strcmp(TEMP_UNIT, "imperial") == 0){
                temperature = temperature * 9. / 5. + 32.; //fahrenheit
            }
            currentWeather.temperature = temperature;
            currentWeather.weatherConditionCode = 800;
        }
        weatherIntervalCounter = 0;
    }else{
        weatherIntervalCounter++;
    }
    return currentWeather;
}

float Watchy::getBatteryVoltage(){
    return analogRead(ADC_PIN) / 4096.0 * 7.23;
}

uint16_t Watchy::_readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)address, (uint8_t)len);
    uint8_t i = 0;
    while (Wire.available()) {
        data[i++] = Wire.read();
    }
    return 0;
}

uint16_t Watchy::_writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data, len);
    return (0 !=  Wire.endTransmission());
}

void Watchy::_bmaConfig(){
 
    if (sensor.begin(_readRegister, _writeRegister, delay) == false) {
        //fail to init BMA
        return;
    }

    // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor.setAccelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    sensor.enableAccel();

    struct bma4_int_pin_config config ;
    config.edge_ctrl = BMA4_LEVEL_TRIGGER;
    config.lvl = BMA4_ACTIVE_HIGH;
    config.od = BMA4_PUSH_PULL;
    config.output_en = BMA4_OUTPUT_ENABLE;
    config.input_en = BMA4_INPUT_DISABLE;
    // The correct trigger interrupt needs to be configured as needed
    sensor.setINTPinConfig(config, BMA4_INTR1_MAP);

    struct bma423_axes_remap remap_data;
    remap_data.x_axis = 1;
    remap_data.x_axis_sign = 0xFF;
    remap_data.y_axis = 0;
    remap_data.y_axis_sign = 0xFF;
    remap_data.z_axis = 2;
    remap_data.z_axis_sign = 0xFF;
    // Need to raise the wrist function, need to set the correct axis
    sensor.setRemapAxes(&remap_data);

    // Enable BMA423 isStepCounter feature
    sensor.enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    sensor.enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    sensor.enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    sensor.resetStepCounter();

    // Turn on feature interrupt
    sensor.enableStepCountInterrupt();
    sensor.enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    sensor.enableWakeupInterrupt();  
}


void Watchy::_configModeCallback (WiFiManager *myWiFiManager) {
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setCursor(0, 30);
  display.println("Connect to");
  display.print("SSID: ");
  display.println(WIFI_AP_SSID);
  display.print("IP: ");
  display.println(WiFi.softAPIP());
  display.display(false); //full refresh
  display.hibernate();
}

bool Watchy::connectWiFi(){
    if(WL_CONNECT_FAILED == WiFi.begin()){//WiFi not setup, you can also use hard coded credentials with WiFi.begin(SSID,PASS);
        WIFI_CONFIGURED = false;
    }else{
        if(WL_CONNECTED == WiFi.waitForConnectResult()){//attempt to connect for 10s
            WIFI_CONFIGURED = true;
        }else{//connection failed, time out
            WIFI_CONFIGURED = false;
            //turn off radios
            WiFi.mode(WIFI_OFF);
            btStop();
        }
    }
    return WIFI_CONFIGURED;
}

/***************** fast menu *****************/
/* SAVE FOR FUTURE REFERENCE
  bool timeout = false;
  long lastTimeout = millis();
  pinMode(MENU_BTN_PIN, INPUT);
  pinMode(BACK_BTN_PIN, INPUT);
  pinMode(UP_BTN_PIN, INPUT);
  pinMode(DOWN_BTN_PIN, INPUT);
  while(!timeout){
      if(millis() - lastTimeout > 5000){
          timeout = true;
      }else{
          if(digitalRead(MENU_BTN_PIN) == 1){
            lastTimeout = millis();  
            if(guiState == MAIN_MENU_STATE){//if already in menu, then select menu item
                switch(menuIndex)
                {
                    case 0:
                    showBattery();
                    break;
                    case 1:
                    showBuzz();
                    break;          
                    case 2:
                    showAccelerometer();
                    break;
                    case 3:
                    setTime();
                    break;
                    case 4:
                    setupWifi();
                    break;                    
                    case 5:
                    showUpdateFW();
                    break;
                    default:
                    break;                              
                }
            }else if(guiState == FW_UPDATE_STATE){
                updateFWBegin();
            }
          }else if(digitalRead(BACK_BTN_PIN) == 1){
            lastTimeout = millis();
            if(guiState == MAIN_MENU_STATE){//exit to watch face if already in menu
            RTC.alarm(ALARM_2); //resets the alarm flag in the RTC
            RTC.read(currentTime);
            showWatchFace(false);
            break; //leave loop
            }else if(guiState == APP_STATE){
            showMenu(menuIndex, false);//exit to menu if already in app
            }else if(guiState == FW_UPDATE_STATE){
            showMenu(menuIndex, false);//exit to menu if already in app
            }            
          }else if(digitalRead(UP_BTN_PIN) == 1){
            lastTimeout = millis();
            if(guiState == MAIN_MENU_STATE){//increment menu index
            menuIndex--;
            if(menuIndex < 0){
                menuIndex = MENU_LENGTH - 1;
            }    
            showFastMenu(menuIndex);
            }            
          }else if(digitalRead(DOWN_BTN_PIN) == 1){
            lastTimeout = millis();
            if(guiState == MAIN_MENU_STATE){//decrement menu index
            menuIndex++;
            if(menuIndex > MENU_LENGTH - 1){
                menuIndex = 0;
            }
            showFastMenu(menuIndex);
            }         
          }
      }
  }

  */

