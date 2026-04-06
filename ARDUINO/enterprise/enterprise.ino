/*
  This example connects to a WPA2 Enterprise WiFi network.
  Then it prints the MAC address of the WiFi module,
  the IP address obtained, and other network details.

  Based on ConnectWithWPA.ino by dlf (Metodo2 srl) and Tom Igoe
*/

//Example file has been separated into headers and source files for better organization and readability


#include "enterprise.h"

#include "arduino_secrets.h"

WiFiClient client;
char server[] = SECRET_IP; //This is my laptop's ipv4 address, will replace with pi eventually

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your WPA2 enterprise network SSID (name)
char user[] = SECRET_USER;  // your WPA2 enterprise username
char pass[] = SECRET_PASS;  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
String machineID = "1";  

void setup() {
  LEDsetup();
  serialConnect();
	enterpriseConnect(ssid, user, pass);
	//serverConnect(server);
  Serial.println("getMACString: " + getMACString())
  if(verifyEquipment(getMACString(), server))
  {
    pixels.show();  
  }
  
}

void loop() {

}
