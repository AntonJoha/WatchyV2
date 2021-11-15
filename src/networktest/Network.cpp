#include "WiFi.h"
#include "../config.h"

WiFiServer server(80);

static void Network::handleNetwork(char * (*webpage)(void), void (*handleResponse)(char *)){

	WiFi.mode(WIFI_AP);

	WiFi.softAP(WIFI_AP_SSID, NULL);
	
	IPAddress IP = WiFi.softAPIP();
	
	server.begin();
	
	while (true)
	{
		WiFiClient client = server.available();
	
		while (client.connected())
		{

			char c = client.read();
			
			if (c == '\n')
			{
				
				if (currentLine.length() == 0)


			}

		}

	}

}

