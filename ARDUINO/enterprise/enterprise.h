#pragma once
#include <Arduino.h>
#include <WiFiNINA.h>
#include <SPI.h>


extern int status;
extern WiFiClient client;

void serialConnect();
void enterpriseConnect(const char* ssid, const char* user, const char* pass);
void serverConnect(const char* server);
void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
