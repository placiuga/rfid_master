#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <WS2812-SOLDERED.h>
#include <WiFiNINA.h>
#include "secrets.h"


// SPI pin definitions
#define SCK  (2)   // Clock pin (D13)
#define MOSI (3)   // Data Out pin (D11)
#define MISO (5)   // Data In pin (D12)
#define SS   (4)   // Slave Select (can be any digital pin, using D10 here)
// pin definitions
#define LEDPIN 7
#define SSRPIN 8
#define STATUSLED 0
#define WIFILED 9
#define NUMPIXELS 10
// delay values
#define LOCKTIME 2000 // timeout after unauthorized card is scanned


enum LEDcolors {RED,GREEN,YELLOW,BLUE,AMBER,BLACK};
LEDcolors prevColor;
enum WIFImodes {CLIENT,OFF};
enum ssrStatus {SSR_ON, SSR_OFF};

// WiFi Details
WIFImodes wifiMode = CLIENT; //Change to CLIENT to connect to FSUSecure, or OFF to disable WiFi

char ssid[] = SECRET_SSID; // ssid for connecting as client
char user[] = SECRET_USER; // User for enterprise connection
char pass[] = SECRET_PASS; // pass for ssid
char server[] = "146.201.27.22"; // IP of raspberry pi


byte mac[6]; // mac address of arduino = 78:21:84:7A:2A:B4
WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;


//Card List --- this will be removed after sql connection
uint8_t card1[4] = {0x2B, 0xC3, 0xB1, 0xFB}; // currently this is set as the "good" card
uint8_t card2[4] = {0x22, 0x92, 0x37, 0xAE};
uint8_t card3[4] = {0xF2, 0x3F, 0x54, 0xAE};
uint8_t card4[4] = {0x22, 0x54, 0x29, 0xAE};


// Create an instance of the Adafruit_PN532 class
Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);
WS2812 pixels(NUMPIXELS, LEDPIN);


bool authenticated = 0;  //global variable needed for loop
bool lock = 0; // global variable needed for loop
uint8_t prevUID[] = { 0, 0, 0, 0, 0, 0, 0 };

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");
  setLED(BLACK, WIFILED);
  setLED(BLACK, STATUSLED);
  delay(1000);

  pinMode(SSRPIN, OUTPUT); // set ssr pin as output

  // WiFi Setup and connection - Connecting to Router
  if(wifiMode == CLIENT){
    setLED(AMBER, WIFILED);
    WiFi.setHostname("Arduino-group309");
    while(wifiStatus != WL_CONNECTED){
      Serial.println("Connecting to WiFi...");
      wifiStatus = WiFi.beginEnterprise(ssid, user, pass);
      delay(10000); 
    }
    setLED(BLUE, WIFILED); // set WIFILED BLUE
    Serial.print("Connected to WiFi");
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
  }

  // Initialize the PN532 in SPI mode
  nfc.begin();
  pixels.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x module");
    while (1); // Halt
  }

  // Output firmware version to Serial
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Configure the PN532 to read RFID tags
  nfc.SAMConfig();
  nfc.setPassiveActivationRetries(5);
  Serial.println("Waiting for an NFC card ...");
}


void loop(void) {
  if(wifiStatus != WL_CONNECTED){
    setLED(AMBER, WIFILED);
  }

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 1 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on the card type)

  // return to yellow status if needed
  if(authenticated == 0){
    if(lock == 1){
      delay(LOCKTIME);
      lock = 0;
    }
    setLED(YELLOW, STATUSLED);
    setSSR(SSR_OFF);
    delay(50);
  }

  // Look for an NFC card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) { 
    // NFC card found
    Serial.println("Found an NFC card!");
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) { //UID can be 4 or 7 bytes
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }


    if(checkUidChange(uid)){ // checks if the current uid is not equal to previous uid
      setPrevUID(uid);  // sets previous uid to current uid
      Serial.println("");
      Serial.println("Database Check");
// THIS SECTION CHECKS THE SCANNED UID AGAINST THE UID OF CARD1 AND SETS PERMISSION
// REPLACE WITH DATABASE CONNECTION AND SET PERMISSION
      int correctBytes = 0;
      for (uint8_t i = 0; i < uidLength; i++) { //UID can be 4 or 7 bytes
        if(uid[i] == card1[i]){ // CHECK SCANNED UID AGAINST KNOWN CARD ---- REPLACE WITH DATABASE CHECK
            correctBytes++;
        }
        else{
          correctBytes = 0;
        }
      }
      if(correctBytes == uidLength){     
        authenticated = 1;
      }
      else{
        authenticated = 0;
      }
// END OF TEMPORARY SECTION
    }

    Serial.println("");

    if(authenticated == 1){
        setLED(GREEN, STATUSLED);
        setSSR(SSR_ON);
    }
    else{
      lock = 1;
      setLED(RED, STATUSLED);
      setSSR(SSR_OFF);
    }
  
    delay(1000); // Scan delay
  } // this bracket is the end of if success 
  else {
    authenticated = 0; // reset authentication when removing a card
    for(int i = 0; i < 7; i++){ // reset previous uid to allow for a new card to be scanned
      prevUID[i] = 0;
    }
    // PN532 timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
    delay(10);
  }
}



void setLED(LEDcolors color, int led){
  if (color == YELLOW){
      pixels.setPixelColor(led, pixels.Color(60,40,0));
      pixels.show();
      prevColor = YELLOW;
  }
  else if(color == RED){
    if(prevColor == YELLOW){
      delay(500);
      for(int i = 0; i < 2; i++){
        pixels.setPixelColor(led, pixels.Color(0,0,0));
        pixels.show();
        delay(350);
        pixels.setPixelColor(led, pixels.Color(60,40,0));
        pixels.show();
        delay(350);
      }
      pixels.setPixelColor(led, pixels.Color(0,0,0));
      pixels.show();
      delay(350);
    }
      pixels.setPixelColor(led, pixels.Color(50,0,0));
      pixels.show();
      prevColor = RED;
  }
  else if(color == GREEN){
    if(prevColor == YELLOW){
      delay(500);
      for(int i = 0; i < 2; i++){
        pixels.setPixelColor(led, pixels.Color(0,0,0));
        pixels.show();
        delay(350);
        pixels.setPixelColor(led, pixels.Color(60,40,0));
        pixels.show();
        delay(350);
      }
      pixels.setPixelColor(led, pixels.Color(0,0,0));
      pixels.show();
      delay(350);
    }
      pixels.setPixelColor(led, pixels.Color(0,50,0));
      pixels.show();
      prevColor = GREEN;
  }
  else if(color == BLUE){
    pixels.setPixelColor(led, pixels.Color(0,0,50));
    pixels.show();
  }
  else if(color == AMBER){
    for(int i = 160; i > 30; i--){
      pixels.setPixelColor(led, pixels.Color(i+63,i,0));
      pixels.show();
      delay (5);
    }
    for(int i = 30; i < 160; i++){
      pixels.setPixelColor(led, pixels.Color(i+63,i,0));
      pixels.show();
      delay (5);
    }
  }
  else if(color == BLACK){
    pixels.setPixelColor(led, pixels.Color(0,0,1));
    pixels.show();
  }
  else{
    pixels.setPixelColor(led, pixels.Color(0,0,0));
    pixels.show();
  }
}

void setSSR(ssrStatus status){
  if (status == SSR_ON){
    digitalWrite(SSRPIN, HIGH);
  }
  else{
    digitalWrite(SSRPIN, LOW);
  }
}

bool checkUidChange(uint8_t uid[]){
  for (int i = 0; i < 7; i++){
    if(uid[i] != prevUID[i]){
      return true;
    }
  }
  return false;
}

void setPrevUID(uint8_t uid[]){
  for (int i = 0; i < 7; i++){
    prevUID[i] = uid[i];
  }
}



