#pragma once
#include <Arduino.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include "WS2812-SOLDERED.h"
#include <Wire.h>
#include <Adafruit_PN532.h>

#define SCK  (2)   // Clock pin (D13)
#define MOSI (3)   // Data Out pin (D11)
#define MISO (5)   // Data In pin (D12)
#define SS   (4)   // Slave Select (can be any digital pin, using D10 here)
#define SSRPIN 8

extern int status;
extern WiFiClient client;
extern WS2812 pixels;
extern Adafruit_PN532 nfc;

void serialConnect();
void LEDsetup();
void enterpriseConnect(const char* ssid, const char* user, const char* pass);
void serverConnect(const char* server);
void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
String getMACString();
bool sendData(String server, String machineID, String rfid_uid, String action);
bool verifyEquipment(String machineID, String server);
void nfcStartup();
String uidToString(uint8_t *uid, uint8_t uidLength);