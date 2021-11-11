#include "Watchy.h"
#include "app/SetTimeApp.h"
#include "app/BatteryApp.h"
#include "app/AppShowBuzz.h"
#include "watchface/CircleFace.h"


AppFrame* setTimeFactory()
{
  return new SetTime;
}

AppFrame *batteryFactory() { return new BatteryApp;}

AppFrame *showBuzzFactory() { return new ShowBuzz;}
void addFrames()
{
  Watchy::addApp("Set Time", setTimeFactory);
  Watchy::addApp("Check Battery", batteryFactory);
  Watchy::addApp("Show buzz", showBuzzFactory);
}


void setup(){
  
  addFrames();
  Watchy::setFace(new CircleFace);
  
  Watchy::init();
}

void loop(){}
