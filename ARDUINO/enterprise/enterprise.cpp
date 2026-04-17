#include "enterprise.h"

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);
WS2812 pixels(10, 7);

void serialConnect() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

void LEDsetup() {
    pixels.begin();
    pixels.clear();
    pixels.setBrightness(255);
    pixels.setPixelColor(LEDWifi, 255, 255, 0);
    pixels.setPixelColor(LEDAuth, 255, 255, 0);
    pixels.show();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void enterpriseConnect(const char* ssid, const char* user, const char* pass) {
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA2 enterprise network:
        // - You can optionally provide additional identity and CA cert (string) parameters if your network requires them:
        //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
        status = WiFi.beginEnterprise(ssid, user, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }

    if(status == WL_CONNECTED) {
        Serial.println("Connected");
        digitalWrite(LED_BUILTIN, HIGH);
        pixels.setBrightness(255);
        pixels.setPixelColor(LEDWifi, 0, 0, 255);
        pixels.show();
    }

    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
    printCurrentNet();
    printWifiData();

    Serial.println("Connecting to server...");
}

void serverConnect(const char* server)
{
    //Code to connect to "server" 
    if (client.connect(server, 80)) {
        Serial.println("Connected to server:");
        client.print("GET / HTTP/1.1\r\n");
        client.print("Host: ");
        client.print(server);
        client.print("\r\n");
        client.print("Connection: close\r\n");
        client.print("\r\n");
        
    }
    else {
        Serial.println("Connection failed");
        serverConnect(server);
    }

}

void printWifiData() {
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    printMacAddress(mac);
}

void printCurrentNet() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    printMacAddress(bssid);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
}

void printMacAddress(byte mac[]) {
    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0) {
            Serial.print(":");
        }
    }
    Serial.println();
}

bool verifyEquipment(String machineID, String server)
{
    if (client.connect(server.c_str(), 80)) {
		Serial.println("Verifying machineID against server...");
        String url = "/rfid/verify.php?machineID=" + machineID;
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: " + server);
        client.println("Connection: close");
        client.println();

        unsigned long startTime = millis();
        while (client.connected() && !client.available()) {
            if (millis() - startTime > 3000) {
                Serial.println("Timeout waiting for response");
                client.stop();
                return false;
            }
            delay(10);
        }

        String response = "";
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        client.stop();
		Serial.println(response);

        //look for a blank line to know when to end
        int bodyIndex = response.indexOf("\r\n\r\n");
        String body = (bodyIndex != -1) ? response.substring(bodyIndex + 4) : response;
        body.trim(); //remove white space + newlines

        if (body.indexOf("VALID") != -1) {
            Serial.println("MachineID " + machineID + " already exists and is valid");
            return true;
        } else if (body.indexOf("CREATED") != -1) {
            Serial.println("MachineID " + machineID + " did not exist! Machine created and valid");
            return true;
        } else
        {
            Serial.println("FATAL ERROR: MachineID could not be verified or created. Check server connection and database.");
            Serial.println("Response from server: " + body);
            return false;
        }
        
    }
    else {
        client.stop();
        Serial.println("Connection failed");
        return false;
	}

}


//note: in sendData, server will be set to SECRET_IP (which will probably be pi). For testing, put your own IP in
//arduino_secrets.h, or replace server[] in enterprise.ino
bool sendData(String server, String machineID, String rfid_uid, String action) {
    //stop previous just in case
    client.stop();

    //connect to localhost
    if (client.connect(server.c_str(), 80)) {
        //display sent info for debug 
        Serial.println("Sending: MachineID: " + machineID + ", RFID_UID: " + rfid_uid + ", Action: " + action);

        //generate url based on delivered info
        String url = "/rfid/index.php?machineID=" + machineID +
                    "&rfid_uid=" + rfid_uid +
                    "&action=" + action;

        //send info to php
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: " + server);
        client.println("Connection: close");
        client.println();

        //display what was sent for debug
        Serial.println("Request sent: " + url);
        unsigned long startTime = millis();
        while (client.connected() && !client.available()) {
            if (millis() - startTime > 3000) {
                Serial.println("Timeout waiting for response");
                client.stop();
                return false;
            }
            delay(10);
        }

        //read the response from php script
        String response = "";
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        client.stop();
        Serial.println(response);

        //look for a blank line to know when to end
        int bodyIndex = response.indexOf("\r\n\r\n");
        String body = (bodyIndex != -1) ? response.substring(bodyIndex + 4) : response;
        body.trim(); //remove white space + newlines

        //check for authorization response (or denial, or error)
        if (body.indexOf("AUTHORIZED") != -1) {
            digitalWrite(LED_BUILTIN, HIGH);           
            if(action == "start") {
                Serial.println("Access granted");
                pixels.setPixelColor(LEDAuth, 0, 255, 0);         //green LED
                pixels.show();
                digitalWrite(SSRPIN, HIGH);
                return true;
            }
            else if(action == "end") {
                Serial.println("Ending Session");
                pixels.setPixelColor(LEDAuth, 255, 255, 0);
                pixels.show();
                digitalWrite(SSRPIN, LOW);
                return true;
            }

        } 
        else if (body.indexOf("DENIED") != -1) {
            Serial.println("Access denied");
            pixels.setPixelColor(LEDAuth, 255, 0, 0);             //red LED
            pixels.show();
            digitalWrite(SSRPIN, LOW);
            //delay(1000);
            //pixels.setPixelColor(1, 255, 255, 0); 
            //pixels.show();
            return false;
        } 
        else if (body.indexOf("ERROR") != -1) {
            pixels.setPixelColor(LEDAuth, 255, 0, 0); 
            pixels.show();
            digitalWrite(SSRPIN, LOW);
            //delay(1000);
            //pixels.setPixelColor(1, 255, 255, 0); 
            //pixels.show();
            return false;
        } 
        else {
            Serial.println("Unknown response");
            pixels.setPixelColor(LEDAuth, 255, 0, 0); 
            pixels.show();
            digitalWrite(SSRPIN, LOW);
            //delay(1000);
            //pixels.setPixelColor(1, 255, 255, 0); 
            //pixels.show();
            return false;
        }
    }
    else {
        Serial.println("Connection failed");
    }
}

String getMACString()
{
    byte mac[6];
    WiFi.macAddress(mac);
    String macString = "";

    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            macString += "0";
        }
        macString += String(mac[i], HEX);
    }
    macString.toUpperCase();
    return macString;
}

void nfcStartup() {
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.print("Didn't find PN53x board");
        while (1); // halt
    }
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(5);
}

String uidToString(uint8_t *uid, uint8_t uidLength) {
    String uidString = "";

    for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] < 0x10) uidString += "0";  // leading zero
        uidString += String(uid[i], HEX);
    }

    uidString.toUpperCase();
    return uidString;
}

String getMachineID(String machineMAC, String server) {
    client.stop();
    if (client.connect(server.c_str(), 80)) {
        Serial.println("Verifying machineID against server...");
        String url = "/rfid/getMachineID.php?machineMAC=" + machineMAC;
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: " + server);
        client.println("Connection: close");
        client.println();

        unsigned long startTime = millis();
        while (client.connected() && !client.available()) {
            if (millis() - startTime > 3000) {
                Serial.println("Timeout waiting for response");
                client.stop();
                return;
            }
            delay(10);
        }

        // skip headers
        while (client.available()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") break;  // end of headers
        }

        String id = client.readStringUntil('\n');
        id.trim();  // remove whitespace

        return id;
    }
    else {
        client.stop();
        Serial.println("Connection failed");
        return "error";
    }
}