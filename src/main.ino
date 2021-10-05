#include "Watchy.h"
#include "app/SetTimeApp.h"
#include "app/BatteryApp.h"

AppFrame* setTimeFactory()
{
  return new SetTime;
}

AppFrame *batteryFactory() { return new BatteryApp;}

void addFrames()
{
  Watchy::addApp("Set Time", setTimeFactory);
  Watchy::addApp("Check Battery", batteryFactory);
}


void setup(){
  addFrames();
  Watchy::init();
}

void loop(){}
