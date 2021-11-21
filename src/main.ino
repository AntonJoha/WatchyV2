#include "Watchy.h"
#include "app/SetTimeApp.h"
#include "app/BatteryApp.h"
#include "app/AppShowBuzz.h"
#include "app/AppSetupWifi.h"
#include "watchface/CircleFace.h"
#include "networktest/Network.h"

AppFrame* setTimeFactory()
{
  return new SetTime;
}

AppFrame *batteryFactory() { return new BatteryApp;}

AppFrame *showBuzzFactory() { return new ShowBuzz;}

AppFrame * wifiFactory() { return new SetupWifi;}

void addFrames()
{
  Watchy::addApp("Set Time", setTimeFactory);
  Watchy::addApp("Check Battery", batteryFactory);
  Watchy::addApp("Show buzz", showBuzzFactory);
  Watchy::addApp("Wifi setup", wifiFactory);
}


void setup(){
  
 	addFrames();
 	Watchy::setFace(new CircleFace);
  
 	//Specific to test the network
	//Watchy::init();
	Network n;
	n.handleNetwork(null, null);
}

void loop(){}
