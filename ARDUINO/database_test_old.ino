#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <WS2812-SOLDERED.h>
#include <WiFiNINA.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "secrets.h"


// SPI pin definitions
#define SCK  (2)   // Clock pin (D13)
#define MOSI (3)   // Data Out pin (D11)
#define MISO (5)   // Data In pin (D12)
#define SS   (4)   // Slave Select (can be any digital pin, using D10 here)
#define LEDPIN 7
#define NUMPIXELS 10
#define LOCKTIME 2000
#define STATUSLED 4
#define WIFILED 9
#define SSRPIN 8

enum LEDcolors {RED,GREEN,YELLOW,BLUE,AMBER};
LEDcolors prevColor;

enum WIFImodes {AP,CLIENT,OFF};

// WiFi Details
WIFImodes wifiMode = OFF; //Change to enable access point mode, client mode, or turn wifi off

char ssid[] = SECRET_SSID; // ssid for connecting as client
//char user[] = SECRET_USER; // User for enterprise connection
char pass[] = SECRET_PASS; // pass for ssid
//char server[] = ""
int APip[4] = {10,0,0,1}; // host ip when in AP mode
byte mac[6]; // mac address of arduino = 78:21:84:7A:2A:B4
WiFiServer server(80); // server port number
WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;


IPAddress SQLserver_addr = (146,201,27,22); // address of server to connect to
char SQLuser[] = SECRET_SQLuser; // set user to mac address
char SQLpass[] = SECRET_SQLpass;

char query[128];

MySQL_Connection conn((Client *)&client);
MySQL_Cursor cur = MySQL_Cursor(&conn);



//Card List --- this will be removed after sql connection
uint8_t card1[4] = {0x2B, 0xC3, 0xB1, 0xFB};
uint8_t card2[4] = {0x22, 0x92, 0x37, 0xAE};
uint8_t card3[4] = {0xF2, 0x3F, 0x54, 0xAE};
uint8_t card4[4] = {0x22, 0x54, 0x29, 0xAE};

uint8_t authenticated = 0;
uint8_t denied = 0;

// Create an instance of the Adafruit_PN532 class
Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);
WS2812 pixels(NUMPIXELS, LEDPIN);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  pinMode(SSRPIN, OUTPUT);

  // WiFi Setup and connection - Connecting to Router
  if(wifiMode == CLIENT){
    WiFi.setHostname("Arduino1");
    while(wifiStatus != WL_CONNECTED){
      Serial.print("Connecting to WiFi...");
      wifiStatus = WiFi.begin(ssid, pass);
      delay(5000); 
    }
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

    //if(client.connect(server,80)){
      //Serial.println("Connected to server");
    // }
    if(conn.connect(SQLserver_addr, 3306, SQLuser, SQLpass)){
      delay(1000);
      Serial.print("connected to mariaDB");
    }
    else{
      Serial.print("mariaDB connection failed");
      conn.close();
    }
    //WiFi.end();
  }

  // WiFi Access Point Setup
  if (wifiMode == AP){
    WiFi.config(IPAddress(APip[0], APip[1], APip[2], APip[3])); // default is 192.168.4.1 if not configured
    Serial.print("Creating access point named: RFIDRules");
    WiFi.beginAP("RFIDRules","spongebob");
    server.begin();
  };

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
  if(wifiMode == AP){
    if(wifiStatus != WiFi.status()){
      wifiStatus = WiFi.status();

      if(wifiStatus == WL_AP_CONNECTED){
        Serial.println("Device connected to AP");
        setLED(BLUE, WIFILED);
      }
      else{
        Serial.println("Device disconnected from AP");
        setLED(AMBER, WIFILED);
      }
    }

    if(wifiStatus == WL_AP_CONNECTED){
        setLED(BLUE, WIFILED);
    }
    else{
      setLED(AMBER, WIFILED);
    }
    WiFiClient APclient = server.available();

    if(APclient) {
      //setLED(BLUE, WIFILED);
      Serial.println("new client");           // print a message out the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (APclient.connected()) {  
        scanLOOP();
      } // while client
    APclient.stop();
    Serial.println("client disconnected");
    } // if client
  }

  if(wifiMode == CLIENT){
    if(wifiStatus != WiFi.status()){
      wifiStatus = WiFi.status();

      if(wifiStatus == WL_CONNECTED){
        Serial.println("Device connected to AP");
        setLED(BLUE, WIFILED);
      }
      else{
        Serial.println("Device disconnected from AP");
        setLED(AMBER, WIFILED);
      }
    }

    if(wifiStatus == WL_CONNECTED){
        setLED(BLUE, WIFILED);
    }
    else{
      setLED(AMBER, WIFILED);
    }
    
    
    /*MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    cur_mem->execute(query);

    column_names *columns = cur_mem->get_columns();

    do{
      row = cur_mem->get_next_row();
      if(row != NULL){
        
      }
    } */

  }
  
  if(wifiMode == OFF){
    scanLOOP();
  }
}

// HELPER FUNCTIONS -----------------------------------------
void scanLOOP(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on the card type)

  // return to yellow status if needed
  if(denied == 1){
    denied = 0;
    delay(LOCKTIME); // prev 2000
    setLED(YELLOW, STATUSLED);
    delay(50);
  }
  if(authenticated == 0){
    setLED(YELLOW, STATUSLED);
    delay(50);
  }
  authenticated = 0;

  // Look for an NFC card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) { 
    // NFC card found
    Serial.println("Found an NFC card!");
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    
    bool permission = 0;

    if(wifiMode == CLIENT){
      row_values *row = NULL;

      snprintf(query, sizeof(query), "SELECT permission FROM machine_permissions WHERE machine_mac = '%s' AND student_uid = '%s'", mac, uid);
      cur.execute(query);

      row = cur.get_next_row();
      if(row != NULL){
        permission = atoi(row->values[0]) != 0;
      }
      cur.close();
      authenticated++;
    }
    else {
      for (uint8_t i = 0; i < uidLength; i++) {
        Serial.print(" 0x"); Serial.print(uid[i], HEX);

        if(uid[i] == card1[i]){
          authenticated++;
        }
        else{
          authenticated = 0;
        }
      }
    }
      
    Serial.println("");
    if(wifiMode == CLIENT){
      if(permission == 1){
          setLED(GREEN, STATUSLED);
      }
      else{
        denied = 1;
        authenticated = 0;
          setLED(RED, STATUSLED);
      }
    }
    else{
      if(authenticated == uidLength){
        for(int i = 0; i < NUMPIXELS; i++) {
          setLED(GREEN, STATUSLED);
        }
      }
      else{
        denied = 1;
        authenticated = 0;
        for(int i = 0; i < NUMPIXELS; i++) {
          setLED(RED, STATUSLED);
        }
      }
    }
    delay(1000); // Scan delay
  } // if success 
  else {
    // PN532 timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
    delay(10);
  }
}

void setLED(LEDcolors color, int led){
  if (color == YELLOW){
      digitalWrite(SSRPIN, LOW);
      pixels.setPixelColor(led, pixels.Color(60,40,0));
      pixels.show();
      prevColor = YELLOW;
  }
  else if(color == RED){
    digitalWrite(SSRPIN, LOW);
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
    digitalWrite(SSRPIN, HIGH);

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
  else{
    pixels.setPixelColor(led, pixels.Color(0,0,0));
    pixels.show();
  }
}




