#include "Watchy.h"
#include "app/SetTimeApp.h"
#include "app/BatteryApp.h"
#include "app/AppShowBuzz.h"
#include "watchface/BasicFace.h"


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
  for (int i = 0; i < 10; ++i)
  {
    addFrames();
  }
  Watchy::setFace(new BasicFace);
  Watchy::init();
}

void loop(){}
