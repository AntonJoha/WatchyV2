#include <WiFi.h>
#include <string>
#include "Network.h"
#include "../config.h"
#include "../Watchy.h"	
#include <cstring>
WiFiServer server(80);

/*
client.println("HTTP/1.1 200 OK");
			client.println("Content-type:text/html");
			client.println("Connection: close");
			client.println("Content-length: 4");
			client.println();
			client.println("test");
			client.println();
*/


const char * exampleWebPage(){
	return "<p>This is a paragraph</p>";
}

void handleGet(WiFiClient client, const char * (*webPage)(void)){
	
	if (webPage == null) webPage = exampleWebPage;
	
	const char* message = webPage();

	size_t messageLen =  strlen(message);
	
	
	client.println("HTTP/1.1 200 OK");
	client.println("Content-type:text/html");
	client.println("Connection: close");
	client.println("Content-length: " + String(messageLen));
	client.println();
	client.println(message);
	client.println();

}

void handleClient(WiFiClient client,const char * (*webPage)(void), void (*handleResponse)(char *))
{
	
	std::string message = "";

	while (true){
		char c = client.read();
		if (c == '\n' || c == '\0') break;
		message+= c;
	}
	
	auto display = Watchy::getDisplay();

	display->init(0, false); //_initial_refresh to false to prevent full update on init
 	display->setFullWindow();
	display->setFont(&FreeMonoBold9pt7b);
 	display->setCursor(8, 10);
	display->println(message[0]);
 	display->display(true); //partial refresh
 	display->hibernate();



	handleGet(client, webPage);

}

void Network::handleNetwork(const char * (*webpage)(void), void (*handleResponse)(char *)){

	WiFi.mode(WIFI_AP);

	WiFi.softAP(WIFI_AP_SSID, NULL);
	
	IPAddress IP = WiFi.softAPIP();
	
	server.begin();
	
	while (true)
	{
		WiFiClient client = server.available();
		while (client.connected())
		{
			
			handleClient(client, webpage, handleResponse);		
		}

	}

}

