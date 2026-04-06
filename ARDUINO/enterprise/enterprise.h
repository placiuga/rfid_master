#pragma once
#include <Arduino.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include "WS2812-SOLDERED.h"


extern int status;
extern WiFiClient client;
extern WS2812 pixels;

void serialConnect();
void LEDsetup();
void enterpriseConnect(const char* ssid, const char* user, const char* pass);
void serverConnect(const char* server);
void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
String getMACString();
void sendData(String server, String machineID, String rfid_uid, String action);
bool verifyEquipment(String machineID, String server);
