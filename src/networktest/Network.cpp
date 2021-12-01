#include <WiFi.h>
#include "Network.h"
#include "../config.h"
#include "../Watchy.h"	
WiFiServer server(80);


void handleClient(WiFiClient client, char * (*webpage)(void), void (*handleResponse)(char *))
{
	
}

void Network::handleNetwork(char * (*webpage)(void), void (*handleResponse)(char *)){

	WiFi.mode(WIFI_AP);

	WiFi.softAP(WIFI_AP_SSID, NULL);
	
	IPAddress IP = WiFi.softAPIP();
	
	server.begin();
	
	auto display = Watchy::getDisplay();

	display->init(0, false); //_initial_refresh to false to prevent full update on init
 	display->setFullWindow();
	display->setFont(&FreeMonoBold9pt7b);
 	display->setCursor(8, 10);
	display->println("Test");
 	display->display(false); //partial refresh
 	display->hibernate();

	while (true)
	{
		WiFiClient client = server.available();
		while (client.connected())
		{
			
			client.println("HTTP/1.1 200 OK");
			client.println("Content-type:text/html");
			client.println("Connection: close");
			client.println("Content-length: 4");
			client.println();
			client.println("test");
			client.println();
		}

	}

}

